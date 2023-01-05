#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

struct ParseTreeItem {
	uint nextRow;
	uint final;
};

struct Token {
	uint id;
	uint len;
	int astNodeLocation;
};

layout(std430, binding = 0) readonly buffer TokenParseTree {
	ParseTreeItem tokenParseTree[];
};

layout(std430, binding = 1) readonly coherent buffer Source {
	uint source[];
};

layout(std430, binding = 2) writeonly volatile buffer Tokens {
	Token tokens[];
};

#define CHARS_PER_INVOCATION 4
#define ROW_SIZE 256

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
	uint i;
	for(i = 0; i < 3000; i++) {
		uint pos = start + i;
		if (!inbounds(pos, source.length())) {
			break;
		}
		uint token = source[pos];
		ParseTreeItem pti = tokenParseTree[row * ROW_SIZE + token];
		if (pti.final != 0) {
			lastFinal = pti.final;
		}
		if (pti.nextRow == 0) { 
			break; 
		}
		row = pti.nextRow;
	}
	if (beginsToken(start + i)) {
		outToken = lastFinal;
		outLength = i;	
	} else {
		outToken = 0;
		outLength = 0;
	}
}

void parseIdentifiersAndLiterals(in uint pos, inout uint token, out uint len) {
	len = 0;
	if (beginsAlphanumToken(pos)) {
		token = isNumeric(source[pos]) ? 1 : 2;
		while (inbounds(pos + len, source.length()) && isAlphanum(source[pos + len]) && len < 30) {
			// TODO: Check max source length
			len++;
		}
	}
}

void main() {
	uint start = gl_GlobalInvocationID.x * CHARS_PER_INVOCATION;

	for (uint i = 0; i < CHARS_PER_INVOCATION; i++) {
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