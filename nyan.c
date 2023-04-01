#include <malloc.h>
#include <stdio.h>
#include <wchar.h>
#include <fcntl.h>
#include "nyan.h"
#define MEMSIZE 1024

struct jump_pair {
    int jz;
    int jnz;
};

void set_jump_pair(struct nyan_s *nyan, struct jump_pair *jump_pairs, int *jplen) {
    int stack[nyan->len];
    int stack_idx = 0;

    for (int i = 0; i < nyan->len; i++) {
        switch (nyan->commands[i]) {
            case JUMP_ZERO:
                stack[stack_idx] = i;
                stack_idx++;
                break;
            case JUMP_NON_ZERO:
                stack_idx--;
                jump_pairs[*jplen].jz = stack[stack_idx];
                jump_pairs[*jplen].jnz = i;
                (*jplen)++;
                break;
            default:
                // do nothing
                break;
        }
    }

    if (stack_idx > 0) {
        printf("Error: unmatched [ found\n");
    }
    else if (stack_idx < 0) {
        printf("Error: unmatched ] found\n");
    }
}

void run_nyan(struct nyan_s nyan) {\
    int memory[MEMSIZE];
    for (int i = 0; i < MEMSIZE; i++) {
        memory[i] = 0;
    }

    int pointer = 0;

    uintptr_t max_gone = 0;
    uintptr_t min_gone = 0;

    struct jump_pair *jump_pairs = malloc(sizeof(struct jump_pair) * nyan.len);
    int jplen = 0;

    set_jump_pair(&nyan, jump_pairs, &jplen);

    for (int i = 0; i < nyan.len; i++) {
        switch (nyan.commands[i]) {
            case POINTER_ADD:
                pointer++;
                break;
            case POINTER_SUB:
                pointer--;
                break;
            case VALUE_ADD:
                memory[pointer]++;
                break;
            case VALUE_SUB:
                memory[pointer]--;
                break;
            case DEBUG_PRINT:
                printf("{%d}", memory[pointer]);
                break;
            case PRINT:
                printf("%c", memory[pointer]);
                break;
            case JUMP_ZERO:
                if (memory[pointer] == 0) {
                    // loop jump_pairs and find jz=i
                    for (int j = 0; j < jplen; j++) {
                        if (jump_pairs[j].jz == i) {
                            i = jump_pairs[j].jnz;
                            break;
                        }
                    }
                }
                break;
            case JUMP_NON_ZERO:
                if (memory[pointer] != 0) {
                    // loop jump_pairs and find jnz=i
                    for (int j = 0; j < jplen; j++) {
                        if (jump_pairs[j].jnz == i) {
                            i = jump_pairs[j].jz;
                            break;
                        }
                    }
                }
                break;
        }
    }
}

// create a function that returns nyan struct
struct nyan_s parse_nyan(wchar_t* code, int len) {
    struct nyan_s nyan;
    nyan.commands = malloc(len * sizeof(unsigned short));
    nyan.len = len;
    for (int i = 0; i < len; i++) {
        switch (code[i]) {
            case L'?':
                nyan.commands[i] = POINTER_ADD;
                break;
            case L'!':
                nyan.commands[i] = POINTER_SUB;
                break;
            case L'냥':
                nyan.commands[i] = VALUE_ADD;
                break;
            case L'냐':
                nyan.commands[i] = VALUE_SUB;
                break;
            case L'뀨':
                nyan.commands[i] = DEBUG_PRINT;
                break;
            case L'.':
                nyan.commands[i] = PRINT;
                break;
            case L'~':
                nyan.commands[i] = JUMP_ZERO;
                break;
            case L'-':
                nyan.commands[i] = JUMP_NON_ZERO;
                break;
            case L' ':
                // ignore whitespace
                break;
            case L'\n':
                // ignore newlines
                break;
            default:
                wprintf(L"Unknown command: %lc\n", code[i]);
        }
    }
    return nyan;
}