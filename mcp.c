/*
    module  : mcp.c
    version : 1.2
    date    : 10/27/23
*/
#include "mcc.h"

/*
    The task is to read a binary file and print it.
*/

/* --------------------------- V A R I A B L E S --------------------------- */

instruction codes[MAXPRG];

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    int j;
    FILE *fp;
    int32_t m;
    char str[MAXLIN];
    unsigned char op;
    instruction *pc, *pe;

    fprintf(stderr, "MCP  -  compiled at %s on %s", __TIME__, __DATE__);
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
    printf("%.24s\n", str);
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
    for (j = 0; operator_NAMES[j]; j++)
	;
    for (pe = pc, pc = codes; pc < pe; pc++) {
	printf("%d\t", (int)(pc - codes));
	if ((op = pc->op) < j)
	    printf("%-9s", operator_NAMES[op]);
	else
	    printf("%-9c", op);
	if (op < hlt)
	    printf("\t%" PRId64, pc->val);
	printf("\n");
    }
    exit(EXIT_SUCCESS);
}
