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
	int astNodeLocation;
};

struct ChildNode {
    int ref;
    uint codegenVolume;
};

struct AstNode {
	uint nodeToken;
	ChildNode children[4];
	uint volume;
};

layout(std430, binding = 4) readonly buffer AstParseTree {
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

layout(std430, binding = 0) readonly buffer IrCodegen {
    uint irCodegen[];
};

uint codegenPointer(uint nodeToken) {
	return irCodegen[nodeToken];
}

AstNode fetchAstNodeFromChildRef(int childRef) {
	return astNodes[childRef - 1];
}

int appendAstNode(AstNode newNode, uint pos) {
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
	int index = invocation_hash + offset * sign_;

	astNodes[index] = newNode;
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

uint getCodegenLength(uint nodeToken) {
	uint ptr = codegenPointer(nodeToken);
	if (ptr == 0) return 0;
	return irCodegen[ptr + 4];
}

uint getNodeCodegenVolume(int childRef) {
	AstNode node = fetchAstNodeFromChildRef(childRef);
	uint volume = getCodegenLength(node.nodeToken);
	for (uint i = 0; i < 4; i++) {
		if (node.children[i].ref != 0)
			volume += node.children[i].codegenVolume;
	}
	return volume;
}

void handleChildren(out ChildNode children[4], uint lastFinal, in uint matchBuffer[10], out uint volume) {
	uint len1 = astTreeData[lastFinal + 1];
	uint len2 = astTreeData[lastFinal + len1 + 2];
	uint sigTokLoc = lastFinal + len1 + len2 + 3;
	uint workingVolume = 1;

	for (uint j = 0; j < 4; j++) {
		uint childRef = astTreeData[sigTokLoc + j];
		children[j].codegenVolume = 0;
		if (childRef == 0) { 
			children[j].ref = 0;
		} else {
			int loc = tokens[matchBuffer[childRef - 1]].astNodeLocation;
			children[j].ref = loc;
			if (loc > 0) {
				workingVolume += fetchAstNodeFromChildRef(loc).volume;
				children[j].codegenVolume = getNodeCodegenVolume(loc);
			}
		}
	}
	volume = workingVolume;
}

void tryParse(in uint start, out uint outToken, in uint preToken, 
	out ChildNode children[4], out uint outLength, out uint volume) {
	uint row = 0;
	uint lastFinal = 0;
	uint lenAtFinal = 0;
	uint matchBuffer[10];
	uint matchBufferPointer = 0;
	for(uint i = 0; i < 3000; i++) {
		Token token = tokens[start + i];

        // TODO: break on end of buffer

        if (token.id == 0) continue;
		
		matchBuffer[matchBufferPointer++] = start + i;
		ParseTreeItem pti = astParseTree[row * ROW_SIZE + token.id];
		if ((pti.final != 0) && !checkExcludes(pti, start, i, preToken, token.len)) {
			lastFinal = pti.final;
			lenAtFinal = i;
		}

		if (pti.nextRow == 0) break;

		row = pti.nextRow;
        i += token.len > 0 ? token.len - 1: 0;
	}
	if (lastFinal != 0) {
		handleChildren(children, lastFinal, matchBuffer, volume);
	}
	outToken = astTreeData[lastFinal];
	outLength = lenAtFinal + 1;
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
		ChildNode children[4];
		uint pos = uint(start + i + preTokenLen);
		uint volume = 0;
		tryParse(pos, token, preToken, children, len, volume);

        barrier();
        
		if (token != 0) {
			AstNode newNode = AstNode(token, children, volume);
			int astPos = appendAstNode(newNode, pos);
			tokens[pos].id = token;
    		tokens[pos].len = len;
			tokens[pos].astNodeLocation = int(astPos + 1);
			// TODO: Find the smallest length of tokens that need to be cleared.
			// 		 This will get really slow for higher nodes.
			for (uint j = 1; j < len; j++) {
				tokens[pos + j].id = 0;
			}
		}
        barrier();
	    memoryBarrierBuffer();
	}
}