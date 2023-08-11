/*
    module  : mca.c
    version : 1.3
    date    : 08/11/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and produce an assembly file.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

#define TEMPORARY	11

static char *register_NAMES[] = {
    "rax",	/* temp register; return value */
    "rbx",	/* callee-saved */
    "rcx",	/* used to pass 4th argument to functions */
    "rdx",	/* used to pass 3rd argument to functions */
    "rsp",	/* stack pointer */
    "rbp",	/* callee-saved; base pointer */
    "rsi",	/* used to pass 2nd argument to functions */
    "rdi",	/* used to pass 1st argument to functions */
    "r8",	/* used to pass 5th argument to functions */
    "r9",	/* used to pass 6th argument to functions */
    "r10",	/* temporary */
    "r11",
    "r12",	/* callee-saved registers */
    "r13",
    "r14",
    "r15"
};

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    FILE *fp;
    instruction code[MAXPRG], *pc, *pe;

    fprintf(stderr, "MCA  -  compiled at %s on %s", __TIME__, __DATE__);
    fprintf(stderr, " (%s)\n", VERSION);
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
    printf("\tpush %%rbp\n"); /* prologue */
    printf("\tmov %%rsp, %%rbp\n");
    printf("\tsub $208, %%rsp\n");
    for (pe = pc, pc = &code[1]; pc > code && pc < pe; pc++) {
	switch (pc->op) {
	case add:
	case sub:
	case mul:
	case dvd:
	case eql:
	case neq:
	case gtr:
	case geq:
	case lss:
	case leq:
	case neg:
	    break;

	case loadlocal:
	    printf("\tlea %" PRId64 "(%%rbp), %%%s\n", (pc->adr2 + 1) * 8,
		   register_NAMES[pc->adr1]);
	    printf("\tmov (%%%s), %%%s\n", register_NAMES[pc->adr1],
		   register_NAMES[pc->adr1]);
	    break;

	case loadimmed:
	    printf("\tmov $%" PRId64 ", %%%s\n", pc->adr2,
		   register_NAMES[pc->adr1]);
	    break;

	case storlocal:
	    printf("\tlea %" PRId64 "(%%rbp), %%%s\n", (pc->adr2 + 1) * 8,
		   register_NAMES[TEMPORARY]);
	    printf("\tmov %%%s, (%%%s)\n", register_NAMES[pc->adr1],
		   register_NAMES[TEMPORARY]);
	    break;

	default:
#ifdef _MSC_VER
	    __assume(0);
#else
	    __builtin_unreachable();
#endif
	}
    }
    printf("\tmov %%rbp, %%rsp\n"); /* epilogue */
    printf("\tpop %%rbp\n");
    printf("\tret\n");
    exit(EXIT_SUCCESS);
}
