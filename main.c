#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include "nyan.h"

int main() {
    FILE *fp;
    wchar_t *buffer = NULL;
    size_t size = 0;
    int n;

    fp = fopen("test.nyan", "r,ccs=UTF-8");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    setlocale(LC_ALL, ""); // set the locale to the user's default locale

    while ((n = fgetwc(fp)) != WEOF) {
        buffer = (wchar_t*)realloc(buffer, sizeof(wchar_t) * (size + 1));
        if (buffer == NULL) {
            perror("Error allocating memory");
            fclose(fp);
            return EXIT_FAILURE;
        }
        buffer[size++] = n;
    }
    buffer[size] = L'\0';

    fclose(fp);

    struct nyan_s nyan = parse_nyan(buffer, size);
    run_nyan(nyan);

    free(buffer);

    return EXIT_SUCCESS;
}