/*
    module  : mcd.c
    version : 1.2
    date    : 08/06/23
*/
#include "mcc.h"

/*
    The task is to read a text file and convert this to binary.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

#define MAXSTR	80
#define INSCNT	25

/* --------------------------- F U N C T I O N S --------------------------- */

int main(int argc, char *argv[])
{
    int i;
    FILE *fp;
    int64_t l;
    instruction ins;
    char str[MAXSTR], *output = "mcc.out";

    if (argc == 2)
        output = argv[1];
    if ((fp = fopen(output, "wb")) == 0) {
        fprintf(stderr, "%s cannot create\n", output);
        return 1;
    }
    while (scanf("%" SCNd64 "%s %" SCNd64 "%" SCNd64,
                 &l, str, &ins.adr1, &ins.adr2) == 4) {
        for (i = 0; i < INSCNT; i++)
            if (!strcmp(str, operator_NAMES[i]))
                break;
        if (i == INSCNT)
            fprintf(stderr, "%s not found\n", str);
        else {
            ins.op = i;
            fwrite(&ins, sizeof(ins), 1, fp);
        }
    }
    fclose(fp);
    return 0;
}
