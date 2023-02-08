#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

#define LITERAL 1
#define IDENTIFIER 2

// Reserved IR tokens
#define IR_REFERNCE 1
#define IR_SELF_REFERENCE 2
#define IR_INSERSION 3
#define IR_SOURCE_POS_REF 4
#define IR_LITERAL_REF 5

#define IR_OTHER_TOKENS_START 6

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

uint getChildOffset(uint codegenPtr, uint childIndex) {
    return irCodegen[codegenPtr + childIndex];
}

int fetch_ref (uint ref, AstNode node, inout bool isLit) {
    if (ref > 4) return 0;
    int pos = node.children[ref].ref;

    // Traverse through empty nodes
    uint maxOut = 0;
    while (pos > 0 && maxOut < 100
        && codegenPointer(fetchAstNodeFromChildRef(pos).nodeToken) == 0) {

        pos = fetchAstNodeFromChildRef(pos).children[0].ref;

        if (fetchAstNodeFromChildRef(pos).nodeToken == LITERAL) {
            isLit = true;
            return pos;
        }

        maxOut++;
    }

    return pos;
}

void writeToOutput(uint pos, AstNode node, int nodePos) {
    uint ptr = codegenPointer(node.nodeToken) + 5;
    uint len = getCodegenLength(node.nodeToken);
    for (uint i = 0; i < len; i++) {
        // Internal positions
        for (int j = 0; j < 4; j++) {
            if (getChildOffset(ptr - 5, j) == i)
                pos += node.children[j].codegenVolume;
        }

        uint token = irCodegen[ptr + i];

        // TODO: refactor into smaller fuctions
        if (token >= IR_OTHER_TOKENS_START)

            output_[pos + i] = token;

        else if (token == IR_REFERNCE) {

            if (++i >= len) break;
            bool isLit = false;
            int ref = fetch_ref(irCodegen[ptr + i], node, isLit);
            // TODO: deal with refs at 0
            if (isLit) {
                output_[pos + i - 1] = IR_LITERAL_REF;
                output_[pos + i] = ref;
            } else if (ref > 0) {
                output_[pos + i - 1] = IR_REFERNCE;
                output_[pos + i] = ref;
            } else {
                output_[pos + i - 1] = IR_SOURCE_POS_REF;
                output_[pos + i] = -ref;
            }

        } else if (token == IR_SELF_REFERENCE) {

            output_[pos + i] = IR_REFERNCE;
            if (++i >= len) break;
            output_[pos + i] = nodePos;

        } else if (token == IR_INSERSION) {

            i++;
            
        }
    }
}

void main() {
    uint pos = gl_GlobalInvocationID.x;
    uint workingVolume = pos + 1;
    uint wokringStartPos = 0;
    int currentNodePos = tokens[0].astNodeLocation - 1;
    AstNode currentNode = astNodes[currentNodePos];
    if (workingVolume > currentNode.volume) return;


    uint maxOut = 0;
    while(workingVolume != currentNode.volume && maxOut < 1024) {
        for (int i = 0; i < 4; i++) {
            if (currentNode.children[i].ref == 0) continue;
            int newNodePos = currentNode.children[i].ref;
            AstNode childNode = fetchAstNodeFromChildRef(newNodePos);
            uint childLength = currentNode.children[i].codegenVolume;
            if (workingVolume <= childNode.volume) {
                // decend

                uint codegenPtr = codegenPointer(currentNode.nodeToken);
                wokringStartPos += getChildOffset(codegenPtr, i);

                currentNodePos = newNodePos;
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

    writeToOutput(wokringStartPos, currentNode, currentNodePos);
}