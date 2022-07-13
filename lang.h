#pragma once
#include "yacc_parser.h"
#include "shader.h"
#include "ir_parser.h"

#define OPERAND_ORDER_AND_EXCLUSION_LISTS_LENGTH 10
struct parse_tree_extra {
    int operand_order[OPERAND_ORDER_AND_EXCLUSION_LISTS_LENGTH];
    int pre_char_exclusions_counter;
    int pre_char_exclusions[OPERAND_ORDER_AND_EXCLUSION_LISTS_LENGTH];

    friend bool operator==(const parse_tree_extra& lhs, const parse_tree_extra& rhs)
    {
        if (lhs.pre_char_exclusions_counter != rhs.pre_char_exclusions_counter)
            return false;
        for (int i = 0; i < OPERAND_ORDER_AND_EXCLUSION_LISTS_LENGTH; i++) {
            if (lhs.operand_order[i] != rhs.operand_order[i])
                return false;
        }
        for (int i = 0; i < lhs.pre_char_exclusions_counter; i++) {
            if (lhs.pre_char_exclusions[i] != rhs.pre_char_exclusions[i])
                return false;
        }
        return true;
    };
};


#undef OPERAND_ORDER_AND_EXCLUSION_LISTS_LENGTH

class lang
{
    public:
        inline static void regen_lang();
        inline static GLuint vram_token_tree;
        inline static GLuint vram_cst;
        inline static void load_lang(char* preset);
        inline static parse_tree_extra* _parse_tree_extra;

    private:
        inline static token_tree _token_tree;
        inline static token_tree _cst;
        lang() {}
};