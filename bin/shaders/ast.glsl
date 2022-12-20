#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

#define CHARS_PER_INVOCATION 4
#define ROW_SIZE 256
#define MAX_AST_NODES 30

struct ParseTreeItem {
	uint nextRow;
	uint final;
};

struct Token {
	uint id;
	uint len;
};

struct AstNode {
	uint nodeToken;
	uint children[4];
};

layout(std430, binding = 0) readonly buffer AstParseTree {
	ParseTreeItem astParseTree[];
};

layout(std430, binding = 1) readonly buffer AstTreeData {
	uint astTreeData[];
};

layout(std430, binding = 2) coherent buffer Tokens {
	Token tokens[];
};

layout(std430, binding = 3) coherent volatile buffer AstNodes {
	AstNode astNodes[];
};

uint appendAstNode(uint nodeToken, uint children[4], uint pos) {
	int invocation_hash = int(pos * 7 + 1) % MAX_AST_NODES;
	int offset = 0;
	int sign_ = 1;
	for (offset = 0; offset < 40; offset++) {
		if (invocation_hash + offset < MAX_AST_NODES 
		&& astNodes[invocation_hash + offset].nodeToken == 0) {
			break;
		}
		if (invocation_hash > offset 
		&& astNodes[invocation_hash - offset].nodeToken == 0) {
			sign_ = -1;
			break;
		}
	}
	uint index = uint(invocation_hash + offset * sign_);

	astNodes[index] = AstNode(nodeToken, children);
	return index;
}

bool checkExcludes(ParseTreeItem pti, uint start, uint i, uint preToken, uint lastTokenLen) {
	// Check pre exclusions
	uint preExclLen = astTreeData[pti.final + 1];
	for (uint j = 0; j < preExclLen; j++) {
		uint preExcl = astTreeData[pti.final + 2 + j];
		if (preExcl == preToken) {
			return true;
		}
	}

	// Check post exclusions
	uint postTokenPos = start + i + lastTokenLen;
	while (tokens[postTokenPos].id == 0 && postTokenPos < 3000) {
		postTokenPos++;
	}
	uint postToken = tokens[postTokenPos].id;

	uint postExclLen = astTreeData[pti.final + 2 + preExclLen];
	for (uint j = 0; j < postExclLen; j++) {
		uint postExcl = astTreeData[pti.final + 3 + preExclLen + j];
		if (postExcl == postToken) {
			return true;
		}
	}
	return false;
}

void tryParse(in uint start, out uint outToken, in uint preToken, out uint outLength) {
	uint row = 0;
	uint lastFinal = 0;
	uint i;
	for(i = 0; i < 3000; i++) {
		Token token = tokens[start + i];

        // TODO: break on end of buffer

        if (token.id == 0) {
            continue;
        }
		ParseTreeItem pti = astParseTree[row * ROW_SIZE + token.id];
		if ((pti.final != 0) && !checkExcludes(pti, start, i, preToken, token.len)) {
			lastFinal = astTreeData[pti.final];
		}
		if (pti.nextRow == 0) { 
			break; 
		}
		row = pti.nextRow;
        i += token.len > 0 ? token.len - 1: 0;
	}
	outToken = lastFinal;
	outLength = i;
}

void main() {
	int start = int(gl_GlobalInvocationID.x * CHARS_PER_INVOCATION) - 1;
    for (int i = 0; i < CHARS_PER_INVOCATION; i++) {
		// This actually does the next token it just starts on the previous so that
		// we have the length and don't have to iterate backwards to find it
		uint preToken = 0;
		int preTokenLen = 1;
		if (start + i >= 0) {
			preToken = tokens[start + i].id;
			if (preToken == 0) {
				continue;
			}
			preTokenLen = int(tokens[start + i].len);
		}

		uint token = 0;
		uint len = 0;
		uint pos = uint(start + i + preTokenLen);
		tryParse(pos, token, preToken, len);

        barrier();
        
		if (token != 0) {
			appendAstNode(token, uint[](0, 0, 0, 0), pos);
			tokens[pos].id = token;
    		tokens[pos].len = len;
			// TODO: Find the smallest length of tokens that need to be cleared.
			// 		 This will get really slow for higher nodes.
			for (uint j = 1; j < len + 1; j++) {
				tokens[pos + j].id = 0;
				tokens[pos + j].len = 0;
			}
		}
        barrier();
	    memoryBarrierBuffer();
	}
}