/*
    module  : mci.c
    version : 1.2
    date    : 08/06/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and start interpreting.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

#define showcode	true
#define tracing		true

#define topregister	7

/* --------------------------- F U N C T I O N S --------------------------- */

void debug(instruction *pc, instruction *code)
{
    printf("%12" PRId64 "%12.12s%12" PRId64 "%12" PRId64 "\n",
           pc - code, operator_NAMES[pc->op], pc->adr1, pc->adr2);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int64_t reg[topregister + 1];
    instruction code[MAXPRG], *pc, *pe;

    if (argc != 2) {
        fprintf(stderr, "%s <file>\n", argv[0]);
        return 1;
    }
    if ((fp = fopen(argv[1], "rb")) == 0) {
        fprintf(stderr, "%s (file not found)\n", argv[1]);
        return 1;
    }
    pc = &code[1];
    while (fread(pc, sizeof(instruction), 1, fp)) {
        if (showcode)
            debug(pc, code);
        pc++;
    }
    fclose(fp);
    if (tracing)
        printf("interpreting ...\n");
    pe = pc;
    pc = &code[1];
    while (pc > code && pc < pe) {
        if (tracing)
            debug(pc, code);
        switch (pc->op) {
        case loadimmed:
            reg[pc->adr1] = pc->adr2;
            pc++;
            break;

        default:
#ifdef _MSC_VER
            __assume(0);
#else
            __builtin_unreachable();
#endif
        }
    }
    exit(EXIT_SUCCESS);
}
