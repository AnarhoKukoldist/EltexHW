#include <math.h>

float div_f (float a, float b) { // у функции такое название из-за конфликта с функцией из stdlib.h
    if (b == 0.0) {
        return INFINITY;
    }
    else {
       return a / b; 
    }
}