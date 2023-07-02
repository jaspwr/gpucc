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

//INCLUDE ir_tokens

//INCLUDE ast_tokens_generated

//INCLUDE structs

//INCLUDE char_utils

//INCLUDE type_actions.h

//INCLUDE ir_types

layout(std430, binding = 3) readonly coherent buffer AstNodes {
	AstNode astNodes[];
};

layout(std430, binding = 2) readonly buffer Tokens {
	Token tokens[];
};

layout(std430, binding = 1) writeonly volatile buffer Output {
    uint output_[];
};

layout(std430, binding = 5) readonly coherent buffer Source {
	uint source[];
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

bool strcmp(uint lhs_pos, uint rhs_pos) {
    uint i = 0;
    while (true) {
        bool lhs_alphanum = isAlphanum(source[lhs_pos + i]);
        bool rhs_alphanum = isAlphanum(source[rhs_pos + i]);

        if (lhs_alphanum != rhs_alphanum) return false;

        if (!lhs_alphanum && !rhs_alphanum) return true;

        if (source[lhs_pos + i] != source[rhs_pos + i]) return false;

        i++;
    }
    return true;
}

uint fetchIdentifierFromPartialScopeDec(AstNode partialScopeDec, inout uint decFullLoc) {
    AstNode decAssign = fetchAstNodeFromChildRef(partialScopeDec.children[1].ref);
    decFullLoc = decAssign.children[0].ref;
    AstNode decFull = fetchAstNodeFromChildRef(decAssign.children[0].ref);
    AstNode dec = fetchAstNodeFromChildRef(decFull.children[0].ref);
    if (dec.children[2].ref > 0) {
        // This is when the identifier is wrapped in a `primary_expression`.
        // return 7777;
        bool isLit = false;
        return -fetch_ref(2, dec, isLit);
    } else {
        return -dec.children[2].ref;
    }
}

uint findIdentifierVreg(uint str_pos, AstNode start) {
    // 1. ascend to partial_scope || partial_scope_dec
    #define ASCENDING_FOR_PARTIAL 1
    // 2. traverse down until partial_scope_dec
    //      2.1. if found, check if identifier matches
    #define DESCENDING 2
    // 3. if none found, return to pos in 1. and traverse up until scope
    #define ASCENDING_FOR_SCOPE 3
    // 4. repeat from 1.

    uint state = ASCENDING_FOR_PARTIAL;

    AstNode node = start;
    AstNode nodeAtScopeAccent = start;

    for (uint i = 0; i < 100; i++) {

        switch (state) {
            case ASCENDING_FOR_PARTIAL: {

                if (node.nodeToken == AST_partial_scope || node.nodeToken == AST_partial_scope_dec) {
                    state = DESCENDING;
                    nodeAtScopeAccent = node;
                    break;
                }

                if (node.parent == 0) return 0;

                node = astNodes[node.parent];
                break;
            }
            case DESCENDING: {
                if (node.nodeToken == AST_partial_scope_dec) {
                    uint decFullLoc = 0;
                    uint identifier = fetchIdentifierFromPartialScopeDec(node, decFullLoc);
                    // return decFullLoc;
                    //return identifier;
                    if (strcmp(str_pos, identifier)) {
                        return decFullLoc;
                    }
                    // return identifier;
                }

                if (node.children[0].ref == 0) {
                    state = ASCENDING_FOR_SCOPE;
                    node = nodeAtScopeAccent;
                    break;
                }

                node = fetchAstNodeFromChildRef(node.children[0].ref);
                break;
            }
            case ASCENDING_FOR_SCOPE: {
                if (node.nodeToken == AST_scope) {
                    state = ASCENDING_FOR_PARTIAL;
                    break;
                }

                if (node.parent == 0) return 33333;

                node = astNodes[node.parent];
                break;
            }
        }
    }

    return 0;
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
                // output_[pos + i - 1] = IR_SOURCE_POS_REF;
                // output_[pos + i] = -ref;

                output_[pos + i - 1] = IR_REFERNCE;
                uint vreg = findIdentifierVreg(-ref, node);
                output_[pos + i] = vreg;
            }

        } else if (token == IR_SELF_REFERENCE) {

            output_[pos + i] = IR_REFERNCE;
            if (++i >= len) break;
            output_[pos + i] = nodePos;

        } else if (token == IR_INSERSION) {

            i++;


        // FIXME: This can be micro-optimised out by making it traverse
        //        up at the end.
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

#define TYPE_SIGNED true
#define TYPE_UNSIGNED false

uint getTypeBase(uint token, bool signed_, bool long_, bool short_) {
    switch (token) {
        case AST_void:
            return U0;
        case AST_char:
            return signed_ ? I8 : U8;
        case AST_short:
            return signed_ ? I16 : U16;
        case AST_int:
            if (long_) {
                return signed_ ? I64 : U64;
            } else if(short_) {
                return signed_ ? I16 : U16;
            } else {
                return signed_ ? I32 : U32;
            }
        case AST_long:
            return signed_ ? I64 : U64;
        case AST_float:
            return F32;
        case AST_double:
            return long_ ? x86_FP80 : F64;
        default:
            return ERROR_TYPE;
    }
}

void setTypeFromDec(AstNode node, uint vreg) {
    // `node` is a `type_specifier` node.

    uint pointerDepth = 0;
    AstNode currentNode = node;

    bool signed_ = true;
    bool long_ = false;
    bool short_ = false;
    bool volatile_ = false;
    bool const_ = false;

    #define parentNodeToken astNodes[currentNode.parent].nodeToken

    while (parentNodeToken == AST_pointer || parentNodeToken == AST_type_qualifier) {
        if (parentNodeToken == AST_pointer) {
            pointerDepth++;
        } else if (parentNodeToken == AST_type_qualifier) {
            // TODO
        }

        currentNode = astNodes[currentNode.parent];
    }

    // uint baseToken = fetchToken(node, 0);
    uint baseToken = fetchAstNodeFromChildRef(node.children[0].ref).nodeToken;

    vregTypes[vreg].base = getTypeBase(baseToken, signed_, long_, short_);
    vregTypes[vreg].pointer_depth = pointerDepth;
    vregTypes[vreg].load_depth = 1;
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
    uint lastAlloca = 0;

    uint maxOut = 0;
    while(workingVolume != currentNode.volume && maxOut < 1024) {
        for (int i = 0; i < 4; i++) {
            if (currentNode.children[i].ref == 0) continue;
            int newNodePos = currentNode.children[i].ref;
            AstNode childNode = fetchAstNodeFromChildRef(newNodePos);
            uint childLength = currentNode.children[i].codegenVolume;
            if (workingVolume <= childNode.volume) {
                // decend

                if (currentNode.nodeToken == AST_breakable) {
                    lastBreak = currentNodePos;
                } else if (currentNode.nodeToken == AST_continuable) {
                    lastContinue = currentNodePos;
                } else if (currentNode.nodeToken == AST_for_wrapper) {
                    lastContinue = continueOfForWrapper(currentNode);
                } else if (currentNode.nodeToken == AST_declaration_full) {
                    lastAlloca = currentNodePos;
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

    if (currentNode.nodeToken == AST_type_specifier) {
        setTypeFromDec(currentNode, lastAlloca);
    }



    if (codegenPointer(currentNode.nodeToken) == 0) return;
    // NOTE: Nothing after this happens if the node does not add any IR.

    writeToOutput(wokringStartPos, currentNode, currentNodePos, lastContinue, lastBreak);
}
