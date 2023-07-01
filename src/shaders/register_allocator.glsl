#version 460 core

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

//INCLUDE ir_types

//INCLUDE live_intervals

//INCLUDE phys_registers_generated

#define COPY_OPCODE 0x8B

layout(std430, binding = 7) readonly buffer ASM {
    int asm_[];
};


layout(std430, binding = 10) buffer PhysRegMap {
    uint physRegMap[];
};

// God I wish I had enums...

#define IntersectionType uint

#define NO_INTERSECTION 0
#define INTERSECTION 1
#define HEAD_TO_TAIL 2
#define TAIL_TO_HEAD 3

#define PhysRegPool uint

#define DOES_NOT_ALLOCATE 0
#define INT_POOL 1
#define FLOAT_POOL 2

#define INT_POOL_SIZE 4
#define FLOAT_POOL_SIZE 7

PhysRegPool getPhysRegPool(uint vreg) {
    if (vregTypes[vreg].pointer_depth > 0) {
        return INT_POOL;
    }

    uint base = vregTypes[vreg].base;
    // Hopefully this gets optimised.
    switch (base) {
        case U1:
        case U8:
        case U16:
        case U32:
        case U64:
        case I8:
        case I16:
        case I32:
        case I64:
            return INT_POOL;
        case F32:
        case F64:
        case x86_FP80:
            return FLOAT_POOL;
        default:
            return DOES_NOT_ALLOCATE;
    }
}

IntersectionType checkIntersection(uint a_idx, uint b_idx) {
    LiveInterval a = liveIntervals[a_idx];
    LiveInterval b = liveIntervals[b_idx];
    if (a.start < b.start) {
        if (a.end > b.start) {
            return INTERSECTION;
        } else if (a.end == b.start) {
            return HEAD_TO_TAIL;
        } else {
            return NO_INTERSECTION;
        }
    } else {
        if (a.start < b.end) {
            return INTERSECTION;
        } else if (b.end == a.start) {
            return TAIL_TO_HEAD;
        } else {
            return NO_INTERSECTION;
        }
    }
}

#define MAX_NEIGHBORS 16

void findNeighbors(uint selfVreg, inout uint neighbors[MAX_NEIGHBORS], inout uint neighborCount) {
    uint len = liveIntervals.length();
    PhysRegPool selfPool = getPhysRegPool(selfVreg);

    if (selfPool == DOES_NOT_ALLOCATE) {
        return;
    }

    for (uint i = 0; i < len; i++) {
        // TODO: This loop is run a lot; micro-optimise the shit out of it.

        if (i == selfVreg) {
            continue;
        }

        if (selfPool != getPhysRegPool(i)) {
            continue;
        }

        IntersectionType intersectionType = checkIntersection(i, selfVreg);

        if (intersectionType == NO_INTERSECTION) {
            continue;
        }

        if (neighborCount == MAX_NEIGHBORS) {
            return;
        }

        neighbors[neighborCount++] = i;

        bool coalescableHtt = intersectionType == HEAD_TO_TAIL
            && asm_[liveIntervals[selfVreg].start] == COPY_OPCODE;

        bool coalescableTth = intersectionType == TAIL_TO_HEAD
            && asm_[liveIntervals[i].start] == COPY_OPCODE;

        if (coalescableHtt || coalescableTth) {
            // TODO
        }
    }
}

void tryAlloc(uint selfVreg, inout uint neighbors[MAX_NEIGHBORS], inout uint neighborCount) {
    PhysRegPool pool = getPhysRegPool(selfVreg);

    if (pool == DOES_NOT_ALLOCATE) return;

    // Find a free register
    uint reg = 1;
    for (uint i = 0; i < neighborCount; i++) {
        if (physRegMap[neighbors[i]] == reg) {
            reg++;
            i = 0;
        }
    }

    if (pool == INT_POOL) {
        if (reg > INT_POOL_SIZE) {
            return;
        }
        reg += PHYS_REG_rax - 1;
    } else if (pool == FLOAT_POOL) {
        if (reg > FLOAT_POOL_SIZE) {
            return;
        }
        reg += PHYS_REG_xmm0 - 1;
    } else {
        // unreachable
    }

    physRegMap[selfVreg] = reg;
}

void evictIfClash(uint selfVreg, inout uint neighbors[MAX_NEIGHBORS], inout uint neighborCount) {
    uint reg = physRegMap[selfVreg];

    for (uint i = 0; i < neighborCount; i++) {
        if (physRegMap[neighbors[i]] == reg) {
            if (selfVreg > neighbors[i]) {
                // Smaller vreg wins
                physRegMap[selfVreg] = 0;
            }
        }
    }
}

uint max_(uint a, uint b) {
    return a > b ? a : b;
}

void main() {
    uint pos = gl_GlobalInvocationID.x;

    uint neighbors[MAX_NEIGHBORS];
    uint neighborCount = 0;

    uint vreg = pos;

    // if (liveIntervals[vreg].start == 0 && liveIntervals[vreg].end == 0) {
    //     continue;
    // }

    // No live interval -> continue

    // Literal -> continue
    // Label -> continue
    // Loadable -> continue

    findNeighbors(vreg, neighbors, neighborCount);

    barrier();

    uint maxAllocs = max_(INT_POOL_SIZE, FLOAT_POOL_SIZE);
    for (uint i = 0; i < maxAllocs; i++) {
        // allocation phase
        tryAlloc(pos, neighbors, neighborCount);

        barrier();

        // eviction phase
        evictIfClash(pos, neighbors, neighborCount);
    }
}
