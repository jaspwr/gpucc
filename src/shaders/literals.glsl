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
#define MAX_AST_NODES 70

#define LITERAL 1
#define IDENTIFIER 2

//INCLUDE ir_types

//INCLUDE ascii

struct ParseTreeItem {
	uint nextRow;
	uint final;
};

//INCLUDE structs

#define AST_NODES_OVERFLOW_BUFFER_SIZE 50
layout(binding = 0) uniform atomic_uint astNodesOverflowPointer;

layout(std430, binding = 5) readonly coherent buffer Source {
	uint source[];
};

layout(std430, binding = 2) coherent buffer Tokens {
	Token tokens[];
};

layout(std430, binding = 3) coherent volatile buffer AstNodes {
	AstNode astNodes[];
};

bool inbounds(in uint index, in uint len) {
	return index < len;
}

AstNode fetchAstNodeFromChildRef(int childRef) {
	return astNodes[childRef - 1];
}

int appendAstNodeToOverflowBuffer(AstNode newNode) {
	uint index = atomicCounterIncrement(astNodesOverflowPointer);
	if (index >= AST_NODES_OVERFLOW_BUFFER_SIZE) return 0;
	astNodes[index] = newNode;
	return int(index);
}

int appendAstNode(AstNode newNode, uint pos, uint len) {
	// Try find a slot in partition
	for (uint i = 0; i < len; i++) {
		// The overflow buffer size is first in the SSBO so it's size is added here
		uint index = (pos + i + AST_NODES_OVERFLOW_BUFFER_SIZE);
		if (!inbounds(index, astNodes.length())) return 0;
		if (astNodes[index].nodeToken == 0) {
			astNodes[index] = newNode;
			return int(index);
		}
	}

	// Partition was full, append to overflow buffer
	return appendAstNodeToOverflowBuffer(newNode);
}


void formatFloat(in uint integer, in uint fraction, inout uint valueHalf1, inout uint valueHalf2) {
	if (integer == 0 && fraction == 0) {
		valueHalf1 = 0;
		valueHalf2 = 0;
		return;
	}

	float base = 10.0;
	uint exponent = 0;


	float fraction_ = float(fraction);
	while (fraction_ > 1.0) {
		fraction_ /= base;
	}

	float value = float(integer) + fraction_;

	float value_ = value;

	while (value > 1.0) {
		value /= 2.0;
		exponent++;
	}

	for (uint i = 0; i < 20; i++) {
		value_ *= 2.0;
	}

	uint mantissa = uint(value_);

	// uint integer_ = integer;
	// while (integer_ > 0) {
	// 	integer_ >>= 1;
	// 	exponent++;
	// }

	// uint mantissa = integer;

	while ((mantissa & 0x80000000) == 0) {
		mantissa <<= 1;
	}

	valueHalf2 = (exponent - 1) << 23 | mantissa >> 9;
}

bool isInHexLetterRange(uint c) {
	return (c >= ASCII_LOWER_A && c <= ASCII_LOWER_F)
	        || (c >= ASCII_UPPER_A && c <= ASCII_UPPER_F);
}

void parseNumber(inout uint pos, in uint base, inout uint valueHalf1, inout uint valueHalf2, inout uint typeBase) {
	bool pastDecimalPoint = false;
	uint firstPart = 0;
	uint secondPart = 0;

	while (true) {
		bool isHex = base == 16 && isInHexLetterRange(source[pos]);

		if ((source[pos] >= ASCII_0 && source[pos] <= ASCII_9) || isHex) {
			int digit = int(source[pos]) - int(ASCII_0);

			if (isHex) {
				if (source[pos] >= ASCII_LOWER_A && source[pos] <= ASCII_LOWER_F) {
					digit = int(source[pos]) - int(ASCII_LOWER_A) + 10;
				} else if (source[pos] >= ASCII_UPPER_A && source[pos] <= ASCII_UPPER_F) {
					digit = int(source[pos]) - int(ASCII_UPPER_A) + 10;
				} else {
					break;
				}
			}

			if (pastDecimalPoint) {
				secondPart = secondPart * base + uint(digit);
				// TODO: detect overflow
			} else {
				firstPart = firstPart * base + uint(digit);
			}
		} else if (source[pos] == 46 /* . */) {
			pastDecimalPoint = true;
		} else {
			break;
		}
		pos++;
	}

	uint longCount = 0;
	bool unsigned_ = false;

	while (true) {
		if (source[pos] == ASCII_LOWER_L || source[pos] == ASCII_UPPER_L) {
			longCount++;
		} else if (source[pos] == ASCII_LOWER_U || source[pos] == ASCII_UPPER_U) {
			unsigned_ = true;
		} else if (source[pos] == ASCII_LOWER_F || source[pos] == ASCII_UPPER_F) {
		   	pastDecimalPoint = true;
		} else {
			break;
		}
		pos++;
	}

	if (pastDecimalPoint) {
		typeBase = F32;
		formatFloat(firstPart, secondPart, valueHalf1, valueHalf2);
	} else {
		switch (longCount) {
			case 0: typeBase = unsigned_ ? U32 : I32; break;
			case 1: typeBase = unsigned_ ? U64 : I64; break;
			default: typeBase = /* TODO */ I32; break;
		}

		valueHalf1 = 0;
		valueHalf2 = firstPart;
	}
}

IrType parseLiteral(in uint sourcePos, in uint len, out ChildNode childArray[4]) {
	uint type = 0;
	uint valueHalf1 = 0;
	uint valueHalf2 = 0;

	int base = 10;

	uint pos = sourcePos;

	if (source[pos] == 39 /* ' */) {
		type = U8;
		if (source[pos + 1] == 92 /* \ */) {

		} else {
			valueHalf2 = source[pos + 1];
		}
	} else {
		// Non char
		if (source[pos] == ASCII_0) {
			if (source[pos + 1] == ASCII_UPPER_X || source[pos + 1] == ASCII_LOWER_X) {
				base = 16;
				pos += 2;
			} else {
				base = 8;
				pos += 1;
			}
		}
		parseNumber(pos, base, valueHalf1, valueHalf2, type);
	}

	// valueHalf1 = source[pos];

	childArray[0] = ChildNode( 0, valueHalf1 );
	childArray[1] = ChildNode( 0, valueHalf2 );
	childArray[2] = ChildNode( 0, 0 );
	childArray[3] = ChildNode( 0, 0 );

	return IrType(type, 0, 0);
}

void main() {
	int start = int(gl_GlobalInvocationID.x * CHARS_PER_INVOCATION) - 1;
    for (int i = 0; i < CHARS_PER_INVOCATION; i++) {
		uint pos = uint(start + i);

		if (tokens[pos].id != LITERAL) continue;
		ChildNode childArray[4];
		IrType type = parseLiteral(pos, tokens[pos].len, childArray);

		AstNode newNode = AstNode(
			LITERAL,
			childArray,
			1,
			0
		);

		int astPos = appendAstNode(newNode, pos, 1);
		tokens[pos].astNodeLocation = int(astPos + 1);

		vregTypes[astPos + 1] = type;

        // barrier();
	    // memoryBarrierBuffer();
	}
}
