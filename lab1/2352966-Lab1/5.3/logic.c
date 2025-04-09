#include "calc.h"

double strToDouble(const char *str, int *valid)
{
    char *endptr;
    errno = 0;
    double value = strtod(str, &endptr);
    *valid = (endptr == str || *endptr != '\0' || errno == ERANGE) ? 0 : 1;
    return (endptr == str || *endptr != '\0' || errno == ERANGE) ? 0 : value;
}

int calculate(const char *op1Str, char operator, const char * op2Str, double *result)
{
    int valid1 = 1, valid2 = 2;
    double op1 = strcmp(op1Str, "ANS") == 0 ? *result : strToDouble(op1Str, &valid1),
           op2 = strcmp(op2Str, "ANS") == 0 ? *result : strToDouble(op2Str, &valid2);
    if (!valid1 || !valid2)
    {
        printf("SYNTAX ERROR\n");
        return 1;
    }

    switch (operator)
    {
    case '+':
        *result = op1 + op2;
        break;
    case '-':
        *result = op1 - op2;
        break;
    case 'x':
        *result = op1 * op2;
        break;
    case '/':
        if (op2 == 0)
        {
            printf("MATH ERROR\n");
            return 1;
        }
        *result = op1 / op2;
        break;
    case '%':
        if ((int)op2 == 0 || op1 != (int)op1 || op2 != (int)op2)
        {
            printf("MATH ERROR\n");
            return 1;
        }
        *result = (int)op1 % (int)op2;
        break;
    default:
        printf("SYNTAX ERROR\n");
        return 1;
    }
    return 0;
}
