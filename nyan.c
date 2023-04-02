#include <malloc.h>
#include <stdio.h>
#include <wchar.h>
#include <fcntl.h>
#include <stdbool.h>
#include "nyan.h"
#include "mouse.h"
#include "util.h"

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

int run_nyan(struct nyan_s nyan,  int * rv) {
    int *memory = malloc(sizeof(int) * MEMSIZE);
    for (int i = 0; i < MEMSIZE; i++) {
        memory[i] = 0;
    }

    int pointer = 0;
    int module_pointer = 0;

    if (rv == NULL) {
        memory[pointer] = 0;
    } else {
        memory[pointer] = *rv;
    }

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
            case MODULE_POINTER_ADD:
                module_pointer++;
                break;
            case MODULE_POINTER_SUB:
                module_pointer--;
                break;
            case MODULE_RETREIVE:
                {
                    struct mouse* mice = nyan.mice;
                    for (int j = 0; j < nyan.mice_len; j++) {
                        if (mice[j].x == module_pointer) {
                            // call child mice
                            struct nyan_s child_nyan = parse_nyan(mice[j].f);
                            int ret = run_nyan(child_nyan, &memory[pointer]);
                            memory[pointer] = ret;
                            break;
                        }
                    }
                }
                break;
            case MODULE_RETURN:
                // return to parent module
                return memory[pointer];
                break;
        }
    }
    return -1;
}

// create a function that returns nyan struct
struct nyan_s parse_nyan(char* filename) {
    wchar_t* code = NULL;
    size_t len = 0;
    wread_file(&code, &len, filename);

    char* mbuffer = NULL;
    size_t msize;
    char* mousename = remove_nyan_ext(filename);
    strcat(mousename, ".mouse");
    read_file(&mbuffer, &msize, mousename);


    int mlen;
    struct mouse* mice = parse_mouse(mbuffer, &mlen);

    struct nyan_s nyan;
    nyan.commands = calloc(len, sizeof(unsigned int*));
    for (int i = 0; i < len; i++) {
        nyan.commands[i] = IGNORE;
    }
    nyan.len = len;

    nyan.mice = mice;

    nyan.mice_len = mlen;



//    for (int i = 0; i < nyan.mice_len; i++) {
//        int x;
//        int y;
//        memcpy(&x, &nyan.mice[i]->x, sizeof(int));
//        memcpy(&y, &nyan.mice[i]->y, sizeof(int));
//        printf("NYMouse %d: %d, %d, %s\n", i, x, y, nyan.mice[i]->f);
//    }

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
            case L'먕':
                nyan.commands[i] = MODULE_POINTER_ADD;
                break;
            case L'먀':
                nyan.commands[i] = MODULE_POINTER_SUB;
                break;
            case L':':
                nyan.commands[i] = MODULE_RETREIVE;
                break;
            case L';':
                nyan.commands[i] = MODULE_RETURN;
                break;
            case L' ':
                nyan.commands[i] = IGNORE;
                // ignore whitespace
                break;
            case L'\n':
                nyan.commands[i] = IGNORE;
                // ignore newlines
                break;
            default:
                wprintf(L"Unknown command: %lc\n", code[i]);
        }
    }

    free(mbuffer);
    free(mousename);
    free(code);

    return nyan;
}