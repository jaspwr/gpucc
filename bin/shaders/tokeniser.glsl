// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

#define CHARS_PER_INVOCATION 4
#define ROW_SIZE 256

#define LITERAL 1
#define IDENTIFIER 2

struct ParseTreeItem {
	uint nextRow;
	uint final;
};


//INCLUDE structs


layout(std430, binding = 0) readonly buffer TokenParseTree {
	ParseTreeItem tokenParseTree[];
};

layout(std430, binding = 5) readonly coherent buffer Source {
	uint source[];
};

layout(std430, binding = 2) writeonly volatile buffer Tokens {
	Token tokens[];
};

bool isAlpha(in uint c) {
	return c >= 97 && c <= 122  // a-z
		|| c >= 65 && c <= 90	// A-Z
		|| c == 95 				// _
		;
}

bool isNumeric(in uint c) {
	return c >= 48 && c <= 57;	// 0-9
}

bool isAlphanum(in uint c) {
	return isAlpha(c) || isNumeric(c);
}

bool isWhiteSpace(in uint c) {
	return c == 32 || c == 9 || c == 10 || c == 13; // space, tab, newline, carriage return
}

uint preChar(in uint index) {
	return index == 0 ? 0 : source[index - 1];
}

bool beginsAlphanumToken(in uint index) {
	return isAlphanum(source[index]) && !isAlphanum(preChar(index));
}

bool inbounds(in uint index, in uint len) {
	return index < len;
}

uint enumerate_token_types(in uint c) {
	if (c == 0)
		return 0;
	else if (isAlphanum(c))
		return 1;
	else if (isWhiteSpace(c))
		return 2;
	else
		return 3;
}

bool beginsToken(in uint index) {
	uint t1 = enumerate_token_types(preChar(index));
	uint t2 = enumerate_token_types(source[index]);
	return t1 != t2 || t1 == 3 || t2 == 3;
}

void tryParse(in uint start, out uint outToken, out uint outLength) {
	uint row = 0;
	uint lastFinal = 0;
	uint lastLength = 0;
	uint i;
	for(i = 0; i < 1024; i++) {
		uint pos = start + i;
		if (!inbounds(pos, source.length())) {
			return;
		}
		uint token = source[pos];
		ParseTreeItem pti = tokenParseTree[row * ROW_SIZE + token];
		if (pti.final != 0) {
			lastFinal = pti.final;
			lastLength = i + 1;
		}
		if (pti.nextRow == 0) {
			break;
		}
		row = pti.nextRow;
	}
	if (beginsToken(start + lastLength)) {
		outToken = lastFinal;
		outLength = lastLength;
	} else {
		outToken = 0;
		outLength = 0;
	}
}

void parseIdentifiersAndLiterals(in uint pos, inout uint token, out uint len) {
	len = 0;
	if (beginsAlphanumToken(pos)) {
		token = isNumeric(source[pos]) ? LITERAL : IDENTIFIER;
		while (true) {
			// TODO: Check max source length
			if (inbounds(pos + len, source.length()) && isAlphanum(source[pos + len]) && len < 30) {
				len++;
				continue;
			}
			if (token == LITERAL) {
				if (source[pos + len] == 46 /* . */) {
					len++;
					continue;
				}
				if (source[pos + len - 1] == 101 /* e */ || source[pos + len - 1] == 69 /* E */) {
					if (source[pos + len] == 43 /* + */ || source[pos + len] == 45 /* - */) {
						len++;
						continue;
					}
				}
			}
			break;
		}

	}
}

bool parseCharLiteral(in uint pos, out uint len) {
	len = 1;
	if (source[pos] == 39) {
		while (inbounds(pos + len, source.length()) && source[pos + len] != 39) {
			len++;
			if (len > 3) {
				return false;
			}
		}

		tokens[pos].id = LITERAL;
		tokens[pos].len = ++len;
		tokens[pos].astNodeLocation = -int(pos);
		return true;
	}
	return false;
}

void main() {
	uint start = gl_GlobalInvocationID.x * CHARS_PER_INVOCATION;

	for (uint i = 0; i < CHARS_PER_INVOCATION; i++) {
		uint charLen = 0;
		if (parseCharLiteral(start + i, charLen)) {
			i += charLen;
			continue;
		}
		if (!beginsToken(start + i)) {
			continue;
		}
		uint token = 0;
		uint len = 0;
		uint pos = start + i;

		if (!inbounds(pos, tokens.length())) {
			continue;
		}

		tryParse(pos, token, len);


		if (token == 0) {
			parseIdentifiersAndLiterals(pos, token, len);
		}
		// else if not whitespace insert an error token
		if (token == 0) continue;
		len = len > 0 ? len : 1;
		tokens[pos].id = token;
		tokens[pos].len = len;
		tokens[pos].astNodeLocation = -int(pos);
		i += len - 1;
	}
	barrier();
	memoryBarrierBuffer();

}
