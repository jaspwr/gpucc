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


#define IR_switch_case 7
#define IR_switch_default 10
#define IR_sizeof 12
#define IR_i8 13
#define IR_i32 14
#define IR_f32 15
#define IR_void 16
#define IR_ptr 17
#define IR_struct 18
#define IR_union 20
#define IR_enum 22
#define IR_set_counter 23
#define IR_fn_def_arg 25
#define IR_fn 26
#define IR_ALLOCA 29
#define IR_CALL 30
#define IR_replace_with 31
#define IR_GETELEMENTPTR 32
#define IR_COPY 33
#define IR_replace_me 34
#define IR_COMMA 35
#define IR_ADD 36
#define IR_SUB 37
#define IR_MUL 38
#define IR_DIV 39
#define IR_MOD 40
#define IR_SHR 41
#define IR_SHL 42
#define IR_CMP 43
#define IR_LT 44
#define IR_GT 45
#define IR_LTE 46
#define IR_GTE 47
#define IR_EQ 48
#define IR_NE 49
#define IR_AND 50
#define IR_XOR 51
#define IR_OR 52
#define IR_CAST 53
#define IR_0 54
#define IR_1 55
#define IR_LOAD 57
#define IR_STORE 58
#define IR_BOOL_AND 59
#define IR_BOOL_OR 60
#define IR_SELECT 61
#define IR_JZ 62
#define IR_JMP 63
#define IR_switch 64
#define IR_switch_body_end 67
#define IR_BREAK 68
#define IR_NOP 69
#define IR_CONTINUE 70
#define IR_goto_replace 71
#define IR_RET 72
#define IR_gotoable 73

#define BREAKABLE 91
#define CONTINUABLE 92
#define FOR_WRAPPER 93

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

uint continueOfForWrapper(AstNode for_wrapper) {
    return for_wrapper.children[0].ref;
}

void writeToOutput(uint pos, AstNode node, int nodePos, uint lastContinue, uint lastBreak) {
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
        if (token >= IR_OTHER_TOKENS_START
            && token != IR_BREAK && token != IR_CONTINUE)

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

        } else if (token == IR_BREAK) {
            output_[pos + i] = IR_REFERNCE;
            i++;
            output_[pos + i] = lastBreak;
        } else if (token == IR_CONTINUE) {
            output_[pos + i] = IR_REFERNCE;
            i++;
            output_[pos + i] = lastContinue;
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

    uint lastContinue = 0;
    uint lastBreak = 0;

    uint maxOut = 0;
    while(workingVolume != currentNode.volume && maxOut < 1024) {
        for (int i = 0; i < 4; i++) {
            if (currentNode.children[i].ref == 0) continue;
            int newNodePos = currentNode.children[i].ref;
            AstNode childNode = fetchAstNodeFromChildRef(newNodePos);
            uint childLength = currentNode.children[i].codegenVolume;
            if (workingVolume <= childNode.volume) {
                // decend

                if (currentNode.nodeToken == BREAKABLE) {
                    lastBreak = currentNodePos;
                } else if (currentNode.nodeToken == CONTINUABLE) {
                    lastContinue = currentNodePos;
                } else if (currentNode.nodeToken == FOR_WRAPPER) {
                    lastContinue = continueOfForWrapper(currentNode);
                }

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

    writeToOutput(wokringStartPos, currentNode, currentNodePos, lastContinue, lastBreak);
}
