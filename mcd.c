/*
    module  : mcd.c
    version : 1.4
    date    : 08/17/23
*/
#include "mcc.h"

/*
    The task is to read a text file and convert this to binary.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

#define MAXSTR	80

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    int i, j;
    FILE *fp;
    int64_t l;
    instruction ins;
    char str[MAXSTR], *output = "mcc.out";

    fprintf(stderr, "MCD  -  compiled at %s on %s", __TIME__, __DATE__);
    fprintf(stderr, " (%s)\n", VERSION);
    if (argc == 2)
	output = argv[1];
    if ((fp = fopen(output, "wb")) == 0) {
	fprintf(stderr, "%s (cannot create)\n", output);
	return 1;
    }
    for (j = 0; operator_NAMES[j]; j++)
	;
    while (scanf("%" SCNd64 "%s %" SCNd64 "%" SCNd64,
		 &l, str, &ins.adr1, &ins.adr2) == 4) {
	for (i = 0; i < j; i++)
	    if (!strcmp(str, operator_NAMES[i]))
		break;
	if (i == j)
	    fprintf(stderr, "%s not found\n", str);
	else {
	    ins.op = i;
	    fwrite(&ins, sizeof(ins), 1, fp);
	}
    }
    fclose(fp);
    return 0;
}
