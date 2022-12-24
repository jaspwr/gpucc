#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

struct ChildNode {
    int ref;
    uint codegenVolume;
};

struct AstNode {
	uint nodeToken;
	ChildNode children[4];
	uint volume;
};

struct Token {
	uint id;
	uint len;
	int astNodeLocation;
};

layout(std430, binding = 3) readonly buffer AstNodes {
	AstNode astNodes[];
};

layout(std430, binding = 2) readonly buffer Tokens {
	Token tokens[];
};

layout(std430, binding = 1) writeonly volatile buffer Output {
    uint output_[];
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

uint getCodegenLength(uint nodeToken) {
	uint ptr = codegenPointer(nodeToken);
	if (ptr == 0) return 0;
	return irCodegen[ptr + 4];
}

void writeToOutput(uint pos, AstNode node) {
    uint ptr = codegenPointer(node.nodeToken) + 5;
    uint len = getCodegenLength(node.nodeToken);
    for (uint i = 0; i < len; i++) {
        output_[pos + i] = irCodegen[ptr + i];
    }
}

void main() {
    uint pos = gl_GlobalInvocationID.x;
    uint workingVolume = pos + 1;
    uint wokringStartPos = 0;
    AstNode currentNode = astNodes[tokens[0].astNodeLocation - 1];
    if (workingVolume > currentNode.volume) return;


    uint maxOut = 0;
    while(workingVolume != currentNode.volume && maxOut < 3000) {
        for (int i = 0; i < 4; i++) {
            if (currentNode.children[i].ref == 0) continue;
            AstNode childNode = fetchAstNodeFromChildRef(currentNode.children[i].ref);
            uint childLength = currentNode.children[i].codegenVolume;
            if (workingVolume <= childNode.volume) {
                currentNode = childNode;
                break;
            } else {
                workingVolume -= childNode.volume;
                wokringStartPos += childLength;
            }
        }
        maxOut++;
    }
    if (codegenPointer(currentNode.nodeToken) == 0) return;

    // Internal positions
    for (int i = 0; i < 4; i++) {
        wokringStartPos += currentNode.children[i].codegenVolume;
    }

    writeToOutput(wokringStartPos, currentNode);
}