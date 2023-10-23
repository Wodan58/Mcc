/*
    module  : mca.c
    version : 1.6
    date    : 10/23/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and produce an assembly file.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

instruction codes[MAXPRG];

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    FILE *fp;
    int32_t m;
    int64_t hi = 0;
    char str[MAXLIN];
    unsigned char op;
    instruction *pc, *pe;

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
    fread(str, sizeof(int64_t), 2, fp);				/* header */
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
     * Mark the lines that need a label, because they are target of a jump or
     * call. Also look for immediate followed by loadglobl: the highest value
     * determines the amount of data to reserve.
     */
    for (pe = pc, pc = codes; pc < pe; pc++) {
	if (!pc->op && pc[1].op == loadglobl)	/* globl following immediate */
	    if (hi < pc->val)
		hi = pc->val;
	if (pc->op >= jmp && pc->op < ent)
	    codes[pc->val].label = 1;
    }
    /*
     * The file starts with the main function. The data section is allocated
     * and a pointer is stored in ebx.
     */
    printf("\tbits 64\n");
    printf("\tdefault rel\n");
    printf("\tsection .text\n");
    printf("\tglobal main\n");
    printf("main:\n");
    printf("\tlea rbx, [data]\n");
    /*
     * Prior to code generation, all labels must be identified.
     */
    for (pc = codes; pc < pe; pc++) {
	if (pc->label)
	    printf("L%d:", (int)(pc - codes));
	switch (op = pc->op) {
	case loadimmed:			/* load immediate into A */
	    printf("\tmov rax, %" PRId64 "\n", pc->val);
	    break;
	case localadr:			/* calculate local address */
	    if (pc->val < 0)
		printf("\tlea rax, [rbp-%" PRId64 "]\n", -pc->val * 8);
	    else
		printf("\tlea rax, [rbp+%" PRId64 "]\n", pc->val * 8);
	    break;
	case globladr:			/* calculate global address */
	    printf("\tmov rax, [rbx+%" PRId64 "]\n", pc->val * 8);
	    break;
	case jmp:			/* jump to address */
	    printf("\tjmp L%" PRId64 "\n", pc->val);
	    break;
	case jiz:			/* jump if ax is zero */
	    printf("\ttest al, al\n");
	    printf("\tjz L%" PRId64 "\n", pc->val);
	    break;
	case jnz:			/* jump if ax is not zero */
	    printf("\ttest al, al\n");
	    printf("\tjnz L%" PRId64 "\n", pc->val);
	    break;
	case cal:			/* call subroutine */
	    printf("\tcall L%" PRId64 "\n", pc->val);
	    break;
	case ent:			/* make new stack frame */
	    printf("\tpush rbp\n");	/* prologue */
	    printf("\tmov rbp, rsp\n");
	    if (pc->val)
		printf("\tsub rsp, %" PRId64 "\n", pc->val * 8);
	    break;
	case hlt:
	    printf("\tret\n");		/* return from main */
	    break;
	case lev:			/* restore call frame and PC */
	    printf("\tmov rsp, rbp\n");
	    printf("\tpop rbp\n");
	    printf("\tret\n");
	    break;
	case loadlocal:			/* load local integer */
	    printf("\tmov rax, [rbp+rax*8]\n");
	    break;
	case loadglobl:			/* load global integer */
	    printf("\tmov rax, [rbx+rax*8]\n");
	    break;
	case storlocal:			/* store integer local */
	    printf("\tpop rcx\n");
	    printf("\tmov [rbp+rcx*8], rax\n");
	    break;
	case storglobl:			/* store integer global */
	    printf("\tpop rcx\n");
	    printf("\tmov [rbx+rcx*8], rax\n");
	    break;
	case push:			/* push the value of rax onto stack */
	    printf("\tpush rax\n");
	    break;
	case '*':			/* unary operator DEREF */
	    printf("\tmov rax, [rax]\n");
	    break;
	case '-':			/* unary operator MINUS */
	    printf("\tneg rax\n");
	    break;
	case '~':
	    printf("\tnot rax\n");	/* unary operator NOT */
	    break;
	case '!':
	    printf("\ttest rax, rax\n");
	    printf("\tsete al\n");
	    printf("\tmovzx rax, al\n");
	    break;
	case orr:
	    printf("\tpop rcx\n");
	    printf("\tor rax, rcx\n");
	    break;
	case xrr:
	    printf("\tpop rcx\n");
	    printf("\txor rax, rcx\n");
	    break;
	case ann:
	    printf("\tpop rcx\n");
	    printf("\tand rax, rcx\n");
	    break;
	case eql:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    printf("\tsete al\n");
	    break;
	case neq:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    printf("\tsetne al\n");
	    break;
	case lss:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    printf("\tsetl al\n");
	    break;
	case gtr:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    break;
	case leq:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    printf("\tsetle al\n");
	    break;
	case geq:
	    printf("\tpop rcx\n");
	    printf("\tcmp rcx, rax\n");
	    printf("\tsetge al\n");
	    break;
	case shl:
	    printf("\tpop rcx\n");
	    printf("\tsal rcx, rax\n");
	    printf("\tmov rax, rcx\n");
	    break;
	case shr:
	    printf("\tpop rcx\n");
	    printf("\tsar rcx, rax\n");
	    printf("\tmov rax, rcx\n");
	    break;
	case add:
	    printf("\tpop rcx\n");
	    printf("\tadd rax, rcx\n");
	    break;
	case sub:
	    printf("\tpop rcx\n");
	    printf("\tsub rcx, rax\n");
	    printf("\tmov rax, rcx\n");
	    break;
	case mul:
	    printf("\tpop rcx\n");
	    printf("\timul rax, rcx\n");
	    break;
	case dvd:
	    printf("\txor rdx, rdx\n");		/* instead of cqo */
	    printf("\tmov rcx, rax\n");
	    printf("\tpop rax\n");
	    printf("\tidiv rcx\n");		/* quotient in rax */
	    break;
	case mod:
	    printf("\txor rdx, rdx\n");		/* instead of cqo */
	    printf("\tmov rcx, rax\n");
	    printf("\tpop rax\n");
	    printf("\tidiv rcx\n");
	    printf("\tmov rax, rdx\n");		/* move remainder to rax */
	    break;
	case writebool:
	    printf("\tlea rcx, [bfmt+rax*8]\n");
	    printf("\tcall printf\n");
	    break;
	case writeint:
	    printf("\tlea rcx, [ifmt]\n");	/* 1st argument: format str */
	    printf("\tmov rdx, rax\n");		/* 2nd argument: integer */
	    printf("\tcall printf\n");
	    break;
	default:
#ifdef _MSC_VER
	    __assume(0);
#else
	    __builtin_unreachable();
#endif
	    break;
	}
    }
    printf("extern printf\n");			/* at end of text section */
    printf("\tsection .data\n");
    printf("ifmt:\tdb '%%lld',10,0\n");		/* writeint format string */
    printf("bfmt:\tdq b0\n");
    printf("\tdq b1\n");
    printf("b0:\tdb 'FALSE',10,0\n");		/* writebool format strings */
    printf("b1:\tdb 'TRUE',10,0\n");
    printf("\tsection .bss\n");
    printf("data:\tresq %" PRId64 "\n", hi + 1);	/* at least one */
    exit(EXIT_SUCCESS);
}
