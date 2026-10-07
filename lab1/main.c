// author: Danila "akshin_" Axyonov

// Compiler: gcc
// Output: main.out

#include <stdio.h>
#include <assert.h>
#include "include/math_.h"

int main(int argc, char const* argv[]) {
    double x;
    printf("Enter positive real number:\n> ");
    scanf("%lf", &x);
    assert(x > 0. && "x must be positive");

    double sqrt_x = sqrt(x);
    printf("sqrt(%lf) = %lf\n", x, sqrt_x);

    return 0;
}
