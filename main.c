#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "nyan.h"
#include "mouse.h"
#include "util.h"

int main() {
    setlocale(LC_ALL, ""); // set the locale to the user's default locale

    struct nyan_s nyan = parse_nyan("test.nyan");
    run_nyan(nyan, NULL);

    return EXIT_SUCCESS;
}

