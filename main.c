#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "nyan.h"
#include "mouse.h"
#include "util.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, ""); // set the locale to the user's default locale

    char* filename = argv[1];

    struct nyan_s nyan = parse_nyan(filename);
    run_nyan(nyan, NULL);

    return EXIT_SUCCESS;
}

