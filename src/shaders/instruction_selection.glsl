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

#define ROW_SIZE 256

//INCLUDE ir_types

struct ParseTreeItem {
	uint nextRow;
	uint final;
};

layout(std430, binding = 8) readonly buffer Matches {
    ParseTreeItem matches[];
};

layout(std430, binding = 9) readonly buffer TypeChecking {
    uint type_checking[];
};

layout(std430, binding = 1) readonly buffer IR {
    uint ir[];
};

bool inbounds(in uint index, in uint len) {
	return index < len;
}

IrType fetch_type_at(inout uint ptr) {
	uint base = type_checking[ptr++];
	uint pointer_depth = type_checking[ptr++];
	uint load_depth = type_checking[ptr++];
	return IrType(base, pointer_depth, load_depth);
}

bool tryTypeMatches(inout uint type_checking_ptr, inout uint vregs[10], inout uint vregs_count, inout uint replace_loc) {
	uint types_count = type_checking[type_checking_ptr++];
	uint fail_jmp_loc = type_checking[type_checking_ptr++];
	uint replace_loc = type_checking[type_checking_ptr++];

	// TODO: Use brain.

	for (uint j = 0; j < types_count; j++) {
		IrType type_ = fetch_type_at(type_checking_ptr);

		if (type != vregTypes[j]) {
			return false;
		}
	}
	return true;
}

int tryGetVreg(uint pos, uint vregs[10], uint vregs_count) {
	for (int j = 0; j < vregs_count; j++) {
		if (vregs[j] == ir[pos]) {
			return j;
			break;
		}
	}
	return -1;
}

int tryParse(in uint start) {
	uint row = 0;
	uint lastFinal = 0;

    uint vregs[10];
    uint vregs_count = 0;

	uint next_token = 0;
	bool has_next_token = false;

	for(uint i = 0; i < 1024; i++) {
		uint pos = start + i;
		if (!inbounds(pos, ir.length())) {
			return;
		}

		uint token = ir[pos];
		if (has_next_token) {
			token = next_token;
			has_next_token = false;
		}

        if (token == IR_REFERNCE || token == IR_LITERAL_REF) {
			int vreg = tryGetVreg(pos + 1, vregs, vregs_count);
			if (vreg == -1) {
				vreg = vregs_count;
				vregs[vregs_count++] = ir[pos + 1];
			}
			next_token = vreg;
            token = IR_REFERNCE;
        }

		ParseTreeItem pti = matches[row * ROW_SIZE + token];
		if (pti.final != 0) {
			lastFinal = pti.final;
		}
		if (pti.nextRow == 0) {
			break;
		}
		row = pti.nextRow;
	}

    uint type_checking_loc = lastFinal;
	uint type_checking_ptr = type_checking[type_checking_loc];
	uint cases = type_checking[type_checking_ptr++];
	for (uint i = 0; i < cases; i++) {
		uint replace_loc = 0;
		if (tryTypeMatches(type_checking_ptr, vregs, vregs_count, replace_loc)) {
			return int(replace_loc);
		}
	}
	return -1;
}

void main() {

}