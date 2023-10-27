/*
    module  : mcd.c
    version : 1.6
    date    : 10/27/23
*/
#include "mcc.h"

/*
    The task is to read a text file and convert this to binary.
*/

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    time_t t;
    struct tm *tm;
    int i, j;
    FILE *fp;
    int64_t l;
    int32_t m;
    unsigned char op;
    char str[MAXLIN], *output = "input.bin";		/* default output */

    fprintf(stderr, "MCD  -  compiled at %s on %s", __TIME__, __DATE__);
    fprintf(stderr, " (%s)\n", VERSION);
    if (argc == 2)
	output = argv[1];
    if ((fp = fopen(output, "wb")) == 0) {
	fprintf(stderr, "%s (cannot create)\n", output);
	return 1;
    }
    t = time(0);
    tm = localtime(&t);
    strftime(str, MAXLIN, "%c", tm);			/* no capitals please */
    fprintf(fp, "%.24s", str);				/* header */
    for (j = 0; operator_NAMES[j]; j++)
	;
    while (scanf("%" SCNd64 "%s", &l, str) == 2) {
	i = op = str[0];
	if (isalpha(op)) {
	    for (i = 0; i < j; i++)
		if (!strcmp(str, operator_NAMES[i]))
		    break;
	    if (i == j) {
		fprintf(stderr, "%s not found\n", str);
		goto done;
	    }
	    op = i;
	}
	fwrite(&op, 1, 1, fp);				/* opcode */
	if (i < hlt) {
	    scanf("%" SCNd64, &l);
	    if (i < jmp)				/* immediates */
		fwrite(&l, sizeof(int64_t), 1, fp);	/* full value */
	    else {
		m = l;
		fwrite(&m, sizeof(int32_t), 1, fp);	/* smaller value */
	    }
	}
    }
done:
    fclose(fp);
    return 0;
}
