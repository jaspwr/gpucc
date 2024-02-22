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

//INCLUDE live_intervals


#define LITERAL 1
#define IDENTIFIER 2

// Reserved IR tokens
#define IR_REFERNCE 1
#define IR_SELF_REFERENCE 2
#define IR_INSERSION 3
#define IR_SOURCE_POS_REF 4
#define IR_LITERAL_REF 5

#define IR_OTHER_TOKENS_START 6

layout(std430, binding = 7) readonly buffer ASM {
    int asm_[];
};

void appendVregReference(uint vreg, uint instPos) {
    atomicCompSwap(liveIntervals[vreg].start, 0, instPos);

    atomicMin(liveIntervals[vreg].start, instPos);
    atomicMax(liveIntervals[vreg].end, instPos);
}

uint instPosOf(uint vregRefPos) {
    // TODO: Restructure when asm is restructured.
    for (uint i = 0; i < 5; i++) {
        uint pos = vregRefPos - i;
        if (asm_[pos] > 0) {
            return pos + 1;
        }
    }
    return 0;
}

void main() {
    #define RANGE 3
    uint pos = gl_GlobalInvocationID.x * RANGE;

    for (uint i = 0; i < RANGE; i++) {
        int vreg = -asm_[pos + i];
        if (vreg < 1) {
            continue;
        }

        uint instPos = instPosOf(pos + i);
        if (instPos == 0) {
            continue;
        }

        appendVregReference(vreg, instPos - 1);
    }
}
