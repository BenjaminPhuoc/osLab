#include "calc.h"

#define CACHE_FILE ".cache"

double ANS = 0.0;

int main()
{
    FILE *fp = fopen(CACHE_FILE, "r");
    if (fp)
    {
        fscanf(fp, "%lf", &ANS);
        fclose(fp);
    }

    char input[256];

    while (1)
    {
        printf(">> ");
        if (!fgets(input, sizeof(input), stdin))
            break;
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0)
            continue;

        if (strcmp(input, "EXIT") == 0)
            break;

        char op1Str[64], op2Str[64];
        char operator;
        int scanned = sscanf(input, "%63s %c %63s", op1Str, &operator, op2Str);
        if (scanned != 3)
        {
            printf("SYNTAX ERROR\n");
            continue;
        }

        double result = ANS;
        int status = calculate(op1Str, operator, op2Str, &result);
        if (!status)
        {
            ANS = result;
            fp = fopen(CACHE_FILE, "w");
            if (fp)
            {
                fprintf(fp, "%.2f", ANS);
                fclose(fp);
            }
            if (result != (int)result)
                printf("%.2f\n", result);
            else
                printf("%.0f\n", result);
        }
    }
    return 0;
}