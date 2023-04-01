//
// Created by Minco on 2023-04-01.
//

#ifndef NYANLANG_NYAN_H
#define NYANLANG_NYAN_H

#include <wchar.h>

struct nyan_s {
    unsigned short* commands;
    int len;
};

enum nyan_command {
    POINTER_ADD = 0,
    POINTER_SUB = 1,
    VALUE_ADD = 2,
    VALUE_SUB = 3,
    DEBUG_PRINT = 4,
    PRINT = 5,
    JUMP_ZERO = 6,
    JUMP_NON_ZERO = 7,
};

void run_nyan(struct nyan_s nyan);
struct nyan_s parse_nyan(wchar_t* code, int len);

#endif //NYANLANG_NYAN_H
