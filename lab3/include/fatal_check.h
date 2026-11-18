// author: Danila "akshin_" Axyonov

#ifndef FATAL_CHECK_H
#define FATAL_CHECK_H



#include <stdbool.h>

void fatal_check(bool good, const char* const context);
void openssl_fatal_check(bool good);



#endif
