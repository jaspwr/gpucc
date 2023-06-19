#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

// %x -> vreg_types[x]
// &x -> fetch type from ast however the fuck
// vreg_types[x] <- %x = ...


#define LITERAL 1
#define IDENTIFIER 2

// Reserved IR tokens
#define IR_REFERNCE 1
#define IR_SELF_REFERENCE 2
#define IR_INSERSION 3
#define IR_SOURCE_POS_REF 4
#define IR_LITERAL_REF 5

#define IR_OTHER_TOKENS_START 6

#define ROW_SIZE 256

struct ParseTreeItem {
	uint nextRow;
	uint final;
};

layout(std430, binding = 0) readonly buffer PatMatchParseTree {
	ParseTreeItem patMatchParseTree[];
};

layout(std430, binding = 1) readonly buffer IR {
    uint ir[];
};

bool inbounds(in uint index, in uint len) {
	return index < len;
}

struct Operand {
    bool isLiteral; // false => vreg, true => literal
    uint ref;
};

// Cols:
// 0 -> 15: placeholder type
// 16 -> 31: defined types
// 32 ... : terminal tokens

uint to_col(in uint token, in uint next_token,
            inout Operand operands[16], inout uint operandCount) {

    if (token == IR_REFERNCE) {
        // vreg
        operands[operandCount].isLiteral = false;
        operands[operandCount].ref = next_token;
        operandCount++;
        forced_to_match = getVregType(next_token);
        return operandCount;
    } else if (token == IR_LITERAL_REF) {
        // literal
        operands[operandCount].isLiteral = true;
        operands[operandCount].ref = next_token;
        operandCount++;
        forced_to_match = getLiteralType(next_token);
        return operandCount;
    } else {
        // terminal token
        return token + 32;
    }
}

void propagateTypes(in uint lastFinal, in Operand operands[16]) {

}

void tryParse(in uint start) {
	uint row = 0;
	uint lastFinal = 0;
	uint lastLength = 0;

    Operand operands[16];
    uint operandCount = 0;


	for(uint i = 0; i < 1024; i++) {
		uint pos = start + i;
		if (!inbounds(pos, ir.length())) {
			return;
		}

        uint token = to_col(ir[pos], ir[pos + 1], operands, operandCount);

		ParseTreeItem pti = patMatchParseTree[row * ROW_SIZE + token];
		if (pti.final != 0) {
			lastFinal = pti.final;
			lastLength = i + 1;
            propagateTypes(lastFinal, operands);
		}
		if (pti.nextRow == 0) {
			break;
		}
		row = pti.nextRow;
	}
}


void main() {

}