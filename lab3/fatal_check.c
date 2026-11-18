// author: Danila "akshin_" Axyonov

#include "./include/fatal_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iso646.h>

#include <openssl/err.h>

void fatal_check(bool good, const char* const context) {
    if (not good) {
        perror(context);
        exit(EXIT_FAILURE);
    }
}

void openssl_fatal_check(bool good) {
    if (not good) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}
