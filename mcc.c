/*
    module  : mcc.c
    version : 1.8
    date    : 08/27/23
*/
#include "mcc.h"

/*
    The task is to read a number and store it in the accumulator.

    The number is read from stdin; the generated code is printed to stdout.
*/

/* ----------------------------- D E F I N E S ----------------------------- */

/*
    maximum size of the symbol table. There will be an error when the table is
    exceeded.
*/
#define MAXSYM	100

/* ------------------------------- T Y P E S ------------------------------- */

typedef struct symbol_t {
    char *name;
    int type,	/* 0=boolean, 1=integer, >=2 address */
	parm;	/* number of local variables */
} symbol_t;

/* --------------------------- V A R I A B L E S --------------------------- */

extern FILE *yyin;

int code_idx;
instruction code[MAXPRG];

static int function_idx, global_idx, local_idx;
static symbol_t functions[MAXSYM], globals[MAXSYM], locals[MAXSYM];

/* --------------------------- F U N C T I O N S --------------------------- */

void enterprog(operator op, int64_t adr1, int64_t adr2)
{
    if (++code_idx >= MAXPRG) {
	yyerror("Exceeding code array");
	return;
    }
    code[code_idx].op = op;
    code[code_idx].adr1 = adr1;
    code[code_idx].adr2 = adr2;
}

void enterlocal(int type)
{
    if (local_idx >= MAXSYM) {
	yyerror("Exceeding local symbol table");
	return;
    }
    locals[local_idx].name = yylval.str;
    locals[local_idx].type = type;
    locals[local_idx].parm = 0;
    local_idx++;
}

/*
    lookup locates a symbol in the symbol table. Found = 1 means found in local
    table; found = 0 means found in global table; found = 2 means found in
    function table, and found = -1 means not found at all.
*/
int lookup(char *str, int *found, int *type)
{
    int i;

    for (i = local_idx - 1; i >= 0; i--)
	if (!strcmp(str, locals[i].name)) {
	    *found = 1;
	    *type = locals[i].type;
	    return i;
	}
    for (i = global_idx - 1; i >= 0; i--)
	if (!strcmp(str, globals[i].name)) {
	    *found = 0;
	    *type = globals[i].type;
	    return i;
	}
    for (i = function_idx - 1; i >= 0; i--)
	if (!strcmp(str, functions[i].name)) {
	    *found = 2;
	    *type = functions[i].type;
	    return i;
	}
    *found = *type = -1;
    return -1;
}

int main(int argc, char *argv[])
{
    int i;

#if YYDEBUG
    yydebug = 0;
#endif
    fprintf(stderr, "MCC  -  compiled at %s on %s", __TIME__, __DATE__);
    fprintf(stderr, " (%s)\n", VERSION);
    if (argc == 2)
	if ((yyin = freopen(argv[1], "r", stdin)) == 0) {
	    fprintf(stderr, "failed to open the file '%s'.\n", argv[1]);
	    return 0;
	}
    yyparse();
    if (errorcount) {
	fprintf(stderr, "%d error(s)\n", errorcount);
	for (i = 0; i < local_idx; i++)
	    printf("name = %s, type = %d, parm = %d\n", locals[i].name,
		   locals[i].type, locals[i].parm);
    } else {
	fprintf(stderr, "%d line(s)\n", code_idx);
	for (i = 1; i <= code_idx; i++)
	    printf("%8d%15s%12" PRId64 "%12" PRId64 "\n",
		    i, operator_NAMES[code[i].op], code[i].adr1, code[i].adr2);
    }
}
