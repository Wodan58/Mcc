/*
    module  : mci.c
    version : 1.8
    date    : 10/27/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and start interpreting.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

// #define DEBUG

#define MAXCOD		1000
#define MAXSTK		1000
#define MAXGLB		1000

/* --------------------------- V A R I A B L E S --------------------------- */

/* memory for program */
instruction codes[MAXCOD];

/* memory for locals and globals */
int64_t stack[MAXSTK + 1], globl[MAXGLB];

/* --------------------------- F U N C T I O N S --------------------------- */

#ifdef DEBUG
void debug(instruction *pc)
{
    int j;
    operator op;

    for (j = 0; operator_NAMES[j]; j++)
	;
    if ((op = pc->op) < j)
	printf("%-9s", operator_NAMES[op]);
    else
	printf("%-9c", op);
    if (op < hlt)
	printf("\t%" PRId64, pc->val);
    printf("\n");
}
#endif

int main(int argc, char *argv[])
{
    int i = 0;
    FILE *fp;
    int32_t m;
    instruction *pc;
    unsigned char op;
    char str[MAXLIN];
    int64_t ax, *sp = &stack[MAXSTK], *bp = sp;

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
    fread(str, sizeof(int64_t), 3, fp);				/* header */
    for (pc = codes; fread(&op, 1, 1, fp); pc++)		/* opcode */
	if ((pc->op = op) < hlt) {
	    if (op < jmp)
		fread(&pc->val, sizeof(int64_t), 1, fp);	/* value */
	    else {
		fread(&m, sizeof(int32_t), 1, fp);		/* value */
		pc->val = m;
	    }
	}
    fclose(fp);
    /*
     * code must be ended with the hlt instruction.
     */
    for (pc = codes;;) {
#ifdef DEBUG
	debug(pc);			/* debug print of codes */
#endif
	switch (pc->op) {
	case loadimmed:			/* load immediate into A */
	    ax = pc->val;
	    break;
	case localadr:			/* calculate local address */
	    ax = (int64_t)&bp[pc->val];
	    break;
	case globladr:			/* calculate global address */
	    ax = (int64_t)&globl[pc->val];
	    break;
	case jmp:			/* jump to address */
	    pc = &codes[pc->val];
	    continue;
	case jiz:			/* jump if ax is zero */
	    pc = ax ? pc + 1 : &codes[pc->val];
	    continue;
	case jnz:			/* jump if ax is not zero */
	    pc = ax ? &codes[pc->val] : pc + 1;
	    continue;
	case cal:			/* call subroutine */
	    *--sp = (int64_t)&pc[1];	/* push return address */
	    pc = &codes[pc->val];	/* jump to address */
	    continue;
	case ent:			/* make new stack frame */
	    *--sp = (int64_t)bp;	/* push old base pointer */
	    bp = sp;			/* new base pointer points to old one */
	    sp = sp - pc->val;		/* stack pointer makes room for local */
	    break;
	case hlt:
	    goto done;
	case lev:			/* restore call frame and PC */
	    sp = bp;
	    bp = (int64_t *)*sp++;	/* pop old base pointer */
	    pc = (instruction *)*sp++;	/* pop return address */
	    continue;
	case loadlocal:			/* load local integer */
	    ax = bp[ax];
	    break;
	case loadglobl:			/* load global integer */
	    ax = globl[ax];
	    break;
	case loadadr:			/* load integer from address */
	    ax = *(int64_t *)ax;
	    break;
	case storlocal:			/* store integer local */
	    bp[*sp++] = ax;
	    break;
	case storglobl:			/* store integer global */
	    globl[*sp++] = ax;
	    break;
	case storadr:			/* store integer at address */
	    *(int64_t *)*sp++ = ax;
	    break;
	case push:			/* push the value of ax onto stack */
	    *--sp = ax;
	    break;
	case pushadr:			/* push the address in ax onto stack */
	    *--sp = ax;
	    break;
	case '*':			/* dereference */
	    ax = *(int64_t *)ax;
	    break;
	case '-':			/* unary operator MINUS */
	    ax = -ax;
	    break;
	case '~':			/* bitwise operator NOT */
	    ax = ~ax;
	    break;
	case '!':			/* boolean operator NOT */
	    ax = 1 - ax;
	    break;
	case bit_or:
	    ax = *sp++ | ax;
	    break;
	case bit_xor:
	    ax = *sp++ ^ ax;
	    break;
	case bit_and:
	    ax = *sp++ & ax;
	    break;
	case eql:
	    ax = *sp++ == ax;
	    break;
	case neq:
	    ax = *sp++ != ax;
	    break;
	case lss:
	    ax = *sp++ < ax;
	    break;
	case gtr:
	    ax = *sp++ > ax;
	    break;
	case leq:
	    ax = *sp++ <= ax;
	    break;
	case geq:
	    ax = *sp++ >= ax;
	    break;
	case shl:
	    ax = *sp++ << ax;
	    break;
	case shr:
	    ax = *sp++ >> ax;
	    break;
	case add:
	    ax = *sp++ + ax;
	    break;
	case sub:
	    ax = *sp++ - ax;
	    break;
	case mul:
	    ax = *sp++ * ax;
	    break;
	case dvd:
	    ax = *sp++ / ax;
	    break;
	case mod:
	    ax = *sp++ % ax;
	    break;
	case writebool:
	    putchar('\t');
	    printf(ax ? "TRUE" : "FALSE");
	    break;
	case writeint:
	    putchar('\t');
	    printf("%" PRId64 "\n", ax);
	    break;
	default:
#ifdef _MSC_VER
	    __assume(0);
#else
	    __builtin_unreachable();
#endif
	    break;
	}
	pc++;
    }
done:
    exit(EXIT_SUCCESS);
}
