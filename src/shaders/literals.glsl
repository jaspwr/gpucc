#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

#define CHARS_PER_INVOCATION 4
#define ROW_SIZE 256
#define MAX_AST_NODES 70

#define LITERAL 1
#define IDENTIFIER 2

//INCLUDE ir_types

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

#define ASCII_ZERO 48

void formatFloat(in uint firstPart, in uint secondPart, inout uint valueHalf1, inout uint valueHalf2) {
	float dec = float(secondPart);
	uint shift = 1;
	while (dec >= 1.0) {
		shift *= 10;
		dec /= 10.0;
	}

	uint value = firstPart * shift + secondPart;
	uint exponent = 0;
	while (value >= shift) {
		value /= 10;
		exponent++;
	}
	valueHalf2 = exponent << 23 | value >> 9;
}

void parseNumber(inout uint pos, in uint base, inout uint valueHalf1, inout uint valueHalf2, inout bool isFloat) {
	bool pastDecimalPoint = false;
	uint firstPart = 0;
	uint secondPart = 0;
	while (true) {
		if (source[pos] >= ASCII_ZERO && source[pos] <= 57 /* 9 */) {
			if (pastDecimalPoint) {
				secondPart = secondPart * base + (source[pos] - ASCII_ZERO);
				// TODO: detect overflow
			} else {
				firstPart = firstPart * base + (source[pos] - ASCII_ZERO);
			}
		} else if (source[pos] == 46 /* . */) {
			pastDecimalPoint = true;
		} else {
			break;
		}
		pos++;
	}
	if (pastDecimalPoint) {
		isFloat = true;
		formatFloat(firstPart, secondPart, valueHalf1, valueHalf2);
	} else {
		isFloat = false;
		valueHalf1 = 0;
		valueHalf2 = firstPart;
	}
}


void parseLiteral(in uint sourcePos, in uint len, out ChildNode childArray[4]) {
	int type = -1;
	uint valueHalf1 = 0;
	uint valueHalf2 = 0;

	int base = 10;


	uint pos = sourcePos; // Possibally not needed idk how arg copying works in glsl

	if (source[pos] == 39 /* ' */) {
		type = U8;
		if (source[pos + 1] == 92 /* \ */) {

		} else {
			valueHalf2 = source[pos + 1];
		}
	} else {
		// Non char
		// TODO: make control flow nicer
		if (source[pos] == ASCII_ZERO) {
			if (source[pos + 1] == 120 /* x */) {
				base = 16;
				pos += 2;
			} else {
				base = 8;
				pos += 1;
			}
		}
		bool isFloat = false;
		parseNumber(pos, base, valueHalf1, valueHalf2, isFloat);
		if (isFloat) {
			type = F32;
		} else {
			type = I32;
		}
	}

	// valueHalf1 = source[pos];

	childArray[0] = ChildNode( 0, type );
	childArray[1] = ChildNode( 0, valueHalf1 );
	childArray[2] = ChildNode( 0, valueHalf2 );
	childArray[3] = ChildNode( 0, 0 );
}

void main() {
	int start = int(gl_GlobalInvocationID.x * CHARS_PER_INVOCATION) - 1;
    for (int i = 0; i < CHARS_PER_INVOCATION; i++) {
		uint pos = uint(start + i);

		if (tokens[pos].id != LITERAL) continue;
		ChildNode childArray[4];
		parseLiteral(pos, tokens[pos].len, childArray);

		AstNode newNode = AstNode(
			LITERAL,
			childArray,
			1,
			0
		);

		int astPos = appendAstNode(newNode, pos, 1);
		tokens[pos].astNodeLocation = int(astPos + 1);

        // barrier();
	    // memoryBarrierBuffer();
	}
}