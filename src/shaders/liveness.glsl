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
