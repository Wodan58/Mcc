/*
    module  : mci.c
    version : 1.6
    date    : 10/13/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and start interpreting.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

#define showcode	true
#define tracing		true

#define maxstack	1000
#define topregister	10

/* --------------------------- F U N C T I O N S --------------------------- */

void debug(instruction *pc, instruction *code)
{
    printf("%12" PRId64 "%12.12s%12" PRId64 "%12" PRId64 "\n",
	   pc - code, operator_NAMES[pc->op], pc->adr1, pc->adr2);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    instruction code[MAXPRG], *pc, *pe;
    int64_t stack[maxstack + 1], stacktop = 0;
    int64_t reg[topregister + 1], baseregister = 0;
    int64_t i;

    fprintf(stderr, "MCI  -  compiled at %s on %s", __TIME__, __DATE__);
    fprintf(stderr, " (%s)\n", VERSION);
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
	case add:
	    reg[pc->adr1] += reg[pc->adr2];
	    pc++;
	    break;

	case sub:
	    reg[pc->adr1] -= reg[pc->adr2];
	    pc++;
	    break;

	case mul:
	    reg[pc->adr1] *= reg[pc->adr2];
	    pc++;
	    break;

	case dvd:
	    reg[pc->adr1] /= reg[pc->adr2];
	    pc++;
	    break;
#if 0
	case mdl:
	    reg[pc->adr1] %= reg[pc->adr2];
	    pc++;
	    break;
#endif
	case eql:
	    reg[pc->adr1] = reg[pc->adr1] == reg[pc->adr2];
	    pc++;
	    break;

	case neq:
	    reg[pc->adr1] = reg[pc->adr1] != reg[pc->adr2];
	    pc++;
	    break;

	case gtr:
	    reg[pc->adr1] = reg[pc->adr1] > reg[pc->adr2];
	    pc++;
	    break;

	case geq:
	    reg[pc->adr1] = reg[pc->adr1] >= reg[pc->adr2];
	    pc++;
	    break;

	case lss:
	    reg[pc->adr1] = reg[pc->adr1] < reg[pc->adr2];
	    pc++;
	    break;

	case leq:
	    reg[pc->adr1] = reg[pc->adr1] <= reg[pc->adr2];
	    pc++;
	    break;
#if 0
	case orr:
	    reg[pc->adr1] = reg[pc->adr1] == 1 || reg[pc->adr2] == 1;
	    pc++;
	    break;
#endif
	case neg:
	    reg[pc->adr1] = 1 - reg[pc->adr1];
	    pc++;
	    break;
#if 0
	case loadglobl:
	    reg[pc->adr1] = stack[pc->adr2];
	    pc++;
	    break;
#endif
	case loadlocal:
	    reg[pc->adr1] = stack[pc->adr2 + baseregister];
	    pc++;
	    break;

	case loadimmed:
	    reg[pc->adr1] = pc->adr2;
	    pc++;
	    break;
#if 0
	case storglobl:
	    stack[pc->adr1] = reg[pc->adr2];
	    pc++;
	    break;
#endif
	case storlocal:
	    stack[pc->adr1 + baseregister] = reg[pc->adr2];
	    pc++;
	    break;
#if 0
	case writebool:
	    puts(reg[pc->adr2] == 1 ? "TRUE" : "FALSE");
	    pc++;
	    break;

	case writeint:
	    printf("%12" PRId64 "\n", reg[pc->adr2]);
	    pc++;
	    break;
#endif
	case cal:
	    if (stacktop + pc->adr2 > maxstack) {
		printf("stack overflow, PC=%" PRId64 ", execution aborted\n",
			pc - code);
		exit(EXIT_FAILURE);
	    }
	    stack[stacktop + 1] = baseregister;
	    stack[stacktop + 2] = pc + 1 - code;
	    baseregister = stacktop;
	    stacktop += pc->adr2;
	    pc = &code[pc->adr1];
	    break;

	case ret:
	    stacktop = baseregister;
	    baseregister = stack[stacktop + 1];
	    pc = &code[stack[stacktop + 2]];
	    break;

	case jmp:
	    pc = &code[pc->adr1];
	    break;

	case jiz:
	    if (reg[pc->adr2] == 0)
		pc = &code[pc->adr1];
	    else
		pc++;
	    break;
#if 0
	case hlt:
	    exit(EXIT_SUCCESS);
#endif
	default:
#ifdef _MSC_VER
	    __assume(0);
#else
	    __builtin_unreachable();
#endif
	}
	if (tracing) {
	    printf("i=%" PRId64 " ", stack[0]);
	    printf("\nj=%" PRId64 " ", stack[1]);
	    printf("\nregs: ");
	    for (i = 0; i <= topregister; i++)
		printf("%" PRId64 " ", reg[i]);
	    printf("\n");
	}
    }
    exit(EXIT_SUCCESS);
}
