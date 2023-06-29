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

layout(std430, binding = 5) readonly buffer Replacements {
    int replacements[];
};

layout(std430, binding = 1) readonly buffer IR {
    uint ir[];
};

layout(std430, binding = 7) writeonly buffer ASM {
    int asm_[];
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
	replace_loc = type_checking[type_checking_ptr++];

	// TODO: Use brain.

	if (types_count != vregs_count) {
		return false;
	}

	for (uint j = 0; j < types_count; j++) {
		IrType type_ = fetch_type_at(type_checking_ptr);

		if (type_.base == 100) {
			continue;
		}

		if (type_ != vregTypes[vregs[j]]) {
			return false;
		}
	}
	return true;
}

int tryGetVreg(uint pos, uint vregs[10], uint vregs_count) {
	if (vregs_count > 10) {
		return -1;
	}

	for (int j = 0; j < vregs_count; j++) {
		if (vregs[j] == ir[pos]) {
			return j;
		}
	}
	return -1;
}

int tryParse(in uint start, inout uint vregs[10], inout uint vregs_count) {
	uint row = 0;
	int lastFinal = -1;

	vregs_count = 0;

	uint next_token = 0;
	bool has_next_token = false;

	for(uint i = 0; i < 1024; i++) {
		uint pos = start + i;
		if (!inbounds(pos, ir.length())) {
			return -1;
		}

		uint token = ir[pos];

		if (has_next_token) {
			token = next_token;
			has_next_token = false;
		} else {
			if (token == IR_REFERNCE || token == IR_LITERAL_REF) {
				int vreg = tryGetVreg(pos + 1, vregs, vregs_count);
				if (vreg == -1) {
					if (vregs_count > 10) {
						return -1;
					}

					vreg = int(vregs_count);
					vregs[vregs_count++] = ir[pos + 1];
				}

				next_token = vreg + 1;
				has_next_token = true;

				token = IR_REFERNCE;
			}
		}

		ParseTreeItem pti = matches[row * ROW_SIZE + token];
		if (pti.final != 0) {
			lastFinal = int(pti.final);
		}
		if (pti.nextRow == 0) {
			break;
		}
		row = pti.nextRow;
	}

	if (lastFinal == -1) {
		return -1;
	}

	// return lastFinal;

    uint type_checking_loc = lastFinal;
	uint type_checking_ptr = type_checking[type_checking_loc];
	uint cases = type_checking[type_checking_ptr++];

	if (cases > 50) {
		return -1;
	}

	for (uint i = 0; i < cases; i++) {
		uint replace_loc = 0;
		if (tryTypeMatches(type_checking_ptr, vregs, vregs_count, replace_loc)) {
			return int(replace_loc);
		}
	}
	return -1;
}

void writeToAsm(uint ptr, uint vregs[10], uint vregs_count, uint replace_loc) {
	uint len = replacements[replace_loc++];
	for (uint i = 0; i < len; i++) {
		int token = replacements[replace_loc++];
		if (token < 0) {
			// asm_[ptr++] = 333;
			asm_[ptr++] = -int(vregs[-token - 1]);
		} else {
			asm_[ptr++] = int(token);
		}
	}
}

void main() {
    uint vregs[10];
    uint vregs_count = 1;

	#define RANGE 3
	uint pos = gl_GlobalInvocationID.x;
	uint start = pos * RANGE;
	for (int i = RANGE - 1; i > -1; i--) {
		uint checking_at = start + i;

		if (checking_at > 0
		    && ir[checking_at - 1] == IR_REFERNCE
		    || ir[checking_at - 1] == IR_LITERAL_REF) {

			continue;
		}

		int replace_loc = tryParse(checking_at, vregs, vregs_count);
		if (replace_loc != -1) {
			// asm_[checking_at] = replace_loc + 1;
			writeToAsm(start + i, vregs, vregs_count, replace_loc);
		}
	}
}
