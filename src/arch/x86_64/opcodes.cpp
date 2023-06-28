#include "opcodes.h"

#include "../../exception.h"

#include <iostream>
#include <unordered_map>

int opcode_of(const std::string& instruction) {
    std::unordered_map<std::string, int> opcode_map = {
        {"add", 0x01},
        {"push", 0x50},
        {"pop", 0x58},
        {"mov", 0x8B},
        {"sub", 0x29},
        {"and", 0x21},
        {"or", 0x09},
        {"xor", 0x31},
        {"not", 0xF7},
        {"inc", 0xFF},
        {"dec", 0xFF},
        {"mul", 0xF7},
        {"imul", 0xF7},
        {"div", 0xF7},
        {"idiv", 0xF7},
        {"neg", 0xF7},
        {"cmp", 0x39},
        {"test", 0x85},
        {"jmp", 0xE9},
        {"je", 0x0F84},
        {"jne", 0x0F85},
        {"jl", 0x0F8C},
        {"jle", 0x0F8E},
        {"jg", 0x0F8F},
        {"jge", 0x0F8D},
        {"ja", 0x0F87},
        {"jae", 0x0F83},
        {"jb", 0x0F82},
        {"jbe", 0x0F86},
        {"call", 0xE8},
        {"ret", 0xC3},
        {"nop", 0x90},
        {"leave", 0xC9},
        {"int", 0xCD},
        {"hlt", 0xF4},
        {"lea", 0x8D},
        {"syscall", 0x0F05},
        {"shl", 0xD3},
        {"shr", 0xD3},
        {"sar", 0xD3},
        {"rol", 0xD3},
        {"ror", 0xD3},
        {"rcl", 0xD3},
        {"rcr", 0xD3},
        {"rep", 0xF3},
        {"repe", 0xF3},
        {"repz", 0xF3},
        {"repne", 0xF2},
        {"repnz", 0xF2},

        // TODO ...
    };

    if (opcode_map.find(instruction) == opcode_map.end()) {
        throw Exception("Unknown instruction \"" + instruction + "\".");
    }

    return opcode_map[instruction];
}