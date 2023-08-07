/*
    module  : mcc.c
    version : 1.4
    date    : 08/07/23
*/
#include "mcc.h"

/*
    The task is to read a number and store it in the accumulator.

    The number is read from stdin; the generated code is printed to stdout.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

extern FILE *yyin;

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

int main(int argc, char *argv[])
{
    int i;

    if (argc == 2)
        if ((yyin = freopen(argv[1], "r", stdin)) == 0) {
            fprintf(stderr, "failed to open the file '%s'.\n", argv[1]);
            return 0;
        }
    yyparse();
    if (errorcount)
	fprintf(stderr, "%d error(s)\n", errorcount);
    else {
	fprintf(stderr, "%d line(s)\n", code_idx);
        for (i = 1; i <= code_idx; i++)
            printf("%8d%15s%12" PRId64 "%12" PRId64 "\n",
                    i, operator_NAMES[code[i].op], code[i].adr1, code[i].adr2);
    }
}
