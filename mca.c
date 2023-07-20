/*
    module  : mca.c
    version : 1.1
    date    : 07/20/23
*/
#include "mcc.h"
#include "pars.h"

/*
    The task is to read a binary file and produce an assembly file.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

static char *register_NAMES[] = { "rax" };

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    FILE *fp;
    instruction code[MAXPRG], *pc, *pe;

    if (argc != 2) {
        fprintf(stderr, "%s <file>\n", argv[0]);
        return 1;
    }
    if ((fp = fopen(argv[1], "rb")) == 0) {
        fprintf(stderr, "%s (file not found)\n", argv[1]);
        return 1;
    }
    for (pc = &code[1]; fread(pc, sizeof(instruction), 1, fp); pc++)
	;
    fclose(fp);
    printf("\t.globl main\n");
    printf("main:\n");
    for (pe = pc, pc = &code[1]; pc > code && pc < pe; pc++) {
        switch (pc->op) {
        case loadimmed:
            printf("\tmov $%" PRId64 ", %%%s\n", pc->adr2,
                   register_NAMES[pc->adr1]);
            break;

        default:
#ifdef _MSC_VER
            __assume(0);
#else
            __builtin_unreachable();
#endif
        }
    }
    printf("\tret\n");
    exit(EXIT_SUCCESS);
}
