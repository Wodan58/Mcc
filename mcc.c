/*
    module  : mcc.c
    version : 1.1
    date    : 07/19/23
*/
#include "mcc.h"
#include "pars.h"

/*
    The task is to read a number and store it in the accumulator.

    The number is read from stdin; the generated code is printed to stdout.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

int code_idx;
instruction code[MAXPRG];

/* --------------------------- F U N C T I O N S --------------------------- */

void enterprog(operator op, int64_t adr1, int64_t adr2)
{
    if (++code_idx >= MAXPRG) {
        yyerror("Exceeding code array");
        return;
    }
    code[code_idx].op = op;
    code[code_idx].adr1 = adr1;
    code[code_idx].adr2 = adr2;
}

int main()
{
    int i;

    if ((i = yyparse()) == 0) {
        for (i = 1; i <= code_idx; i++)
            printf("%8d%15s%12" PRId64 "%12" PRId64 "\n",
                    i, operator_NAMES[code[i].op], code[i].adr1, code[i].adr2);
    }
}
