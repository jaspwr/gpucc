#include "registers.h"

#define STARTING_ID 1

const char* registers[] = {
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
    "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
};

std::string generate_shader_definitions() {
    std::string definitions = "";
    for (int i = 0; i < 32; i++) {
        definitions += "#define PHYS_REG_"
            + std::string(registers[i])
            + " " + std::to_string(i + STARTING_ID) + "\n";
    }
    return definitions;
}

const char* from_register(int reg) {
    return registers[reg - STARTING_ID];
}
