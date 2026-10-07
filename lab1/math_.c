// author: Danila "akshin_" Axyonov

#include "include/math_.h"

const int ITERATIONS_NUMBER = 100;

double sqrt(double x) {
    double l = 0.,
           r = x + 1.;
    for (int _ = 0; _ < ITERATIONS_NUMBER; ++_) {
        double m = (l + r) * .5;
        if (m * m < x)
            l = m;
        else // m * m > x
            r = m;
    }
    return l;
}
