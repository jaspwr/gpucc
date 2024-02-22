// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of gpucc.
//
// gpucc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// gpucc is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpucc. If not, see <https://www.gnu.org/licenses/>.


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

#define ALIGNMENT 4

//INCLUDE ir_types

//INCLUDE ir_tokens_generated

layout(std430, binding = 1) readonly buffer IR {
    uint ir[];
};

layout(std430, binding = 9) writeonly buffer FramePointers {
    uint framePointers[];
};

uint sizeOfAlloc(uint pos) {
    if (pos < 2) {
        return 0;
    }

    // IR Structure:
    // %X = ALLOCA ...
    uint vreg = uint(ir[pos - 2]);
    IrType t = vregTypes[vreg];
    return sizeOf(t);
}

uint findFramePointer(uint pos) {
    uint ptr = 0;

    while (pos > 0 && ir[pos] != IR_FN_PRELUDE) {
        if (ir[pos] == IR_ALLOCA) {
            if (ptr % ALIGNMENT != 0) {
                ptr += ALIGNMENT - (ptr % ALIGNMENT);
            }
            ptr += sizeOfAlloc(pos);
        }

        pos--;
    }

    return ptr;
}

void main() {
    #define RANGE 20
	uint basePos = gl_GlobalInvocationID.x * RANGE;

    for (uint i = RANGE; i > 0; i--) {
        // TODO: Optimise this so that it doesn't travel backwards
        //       multiple times if there are multiple ALLOCAs in the
        //       range.
        uint pos = basePos + i - 1;

        if (pos > 0 && (ir[pos - 1] == IR_REFERNCE
            || ir[pos - 1] == IR_LITERAL_REF)) {
            continue;
        }

        if (ir[pos] == IR_ALLOCA) {
            uint pointer = findFramePointer(pos);
            framePointers[ir[pos - 2]] = pointer;
        }
    }
}