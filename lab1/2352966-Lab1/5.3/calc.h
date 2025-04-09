#ifndef CALC_H
#define CALC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int calculate(const char *opStr1, char operator, const char * op2Str, double *result);

#endif