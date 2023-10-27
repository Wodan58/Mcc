/*
    module  : mcc.c
    version : 1.11
    date    : 10/27/23
*/
#include "mcc.h"

/*
    The task is to generate code and print it to stdout.
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
    int64_t type;	/* address */
} symbol_t;

/* --------------------------- V A R I A B L E S --------------------------- */

extern FILE *yyin;

int code_idx;
instruction code[MAXPRG];

static int function_idx, global_idx, local_idx;
static symbol_t functions[MAXSYM], globals[MAXSYM], locals[MAXSYM];

/* --------------------------- F U N C T I O N S --------------------------- */

/*
 * loadlocal and loadglobl are preceded by loadimmed, that contains the address.
 * assignment requires this address and not the value at the address. That is
 * why load of value is replaced by a push, constituting the first parameter
 * of the assignment expression. In case of indirection it was an address after
 * all and is replaced by a push of the address.
 */
operator assign(void)
{
    operator op = code[code_idx - 1].op;		/* inspect last op */

    if (op == '*')
	code[code_idx - 1].op = pushadr;		/* push real address */
    else if (op == loadlocal || op == loadglobl)
	code[code_idx - 1].op = push;			/* push rel address */
    return op;
}

void enterprog(operator op, int64_t val)
{
    int index, type;

    if (code_idx >= MAXPRG) {
	yyerror("Exceeding code array");
	return;
    }
    /*
     * An address-of operator is preceded by an immediate and a loadlocal or
     * loadglobl. Both can be replaced by localadr or globladr.
     */
    if (op == '&') {
	if (code[--code_idx].op == loadlocal)
	    code[code_idx - 1].op = localadr;		/* relative address */
	else if (code[code_idx].op == loadglobl)
	    code[code_idx - 1].op = globladr;		/* relative address */
	return;
    }
    /*
     * At function end, patch the function entry with the number of local
     * variables. Also reset the local index.
     */
    if (op == lev) {
	type = local_idx ? -locals[local_idx - 1].type : 0;
	for (index = code_idx - 1; index >= 0; index--)
	    if (code[index].op == ent) {
		code[index].val = type;
		break;
	    }
	for (index = local_idx - 1; index >= 0; index--)
	    if (locals[index].type == -1) {
		local_idx = index;
		break;
	    }
    }
    code[code_idx].op = op;
    code[code_idx++].val = val;
}

static void enterglobal(void)
{
    static int type;

    if (global_idx >= MAXSYM) {
	yyerror("Exceeding global symbol table");
	return;
    }
    globals[global_idx].name = yylval.str;
    globals[global_idx++].type = type++;
}

void enterlocal(void)
{
    int index, type = 1;	/* 1=outside function, 0=inside function */

    /*
     * See if the declaration is within a function. If not, it is a global
     * symbol.
     */
    for (index = code_idx - 1; index >= 0; index--) {
	/*
	 * If a leave is found first, it must be outside a function.
	 */
	if (code[index].op == lev)
	    break;
	/*
	 * If an entry is found and not a leave, it must be within a function.
	 */
	if (code[index].op == ent) {
	    type = 0;
	    break;
	}
    }
    if (type) {
	enterglobal();
	return;
    }
    type = local_idx ? locals[local_idx - 1].type - 1 : -1;
    if (local_idx >= MAXSYM) {
	yyerror("Exceeding local symbol table");
	return;
    }
    locals[local_idx].name = yylval.str;
    locals[local_idx++].type = type;
}

void enterfunction(void)
{
    /*
     * main is the special function where execution starts. At location 0 there
     * is a jump to main.
     */
    if (!strcmp(yylval.str, "main"))
	code[0].val = code_idx;
    if (function_idx >= MAXSYM) {
	yyerror("Exceeding function symbol table");
	return;
    }
    functions[function_idx].name = yylval.str;
    functions[function_idx++].type = code_idx;
}

/*
    lookup locates a symbol in the symbol table. Found = 1 means found in local
    table; found = 2 means found in global table; found = 3 means found in
    function table, and found = 0 means not found at all.
*/
int lookup(char *str, int64_t *type)
{
    int i;

    for (i = local_idx - 1; i >= 0; i--)
	if (!strcmp(str, locals[i].name)) {
	    *type = locals[i].type;
	    return 1;
	}
    for (i = global_idx - 1; i >= 0; i--)
	if (!strcmp(str, globals[i].name)) {
	    *type = globals[i].type;
	    return 2;
	}
    for (i = function_idx - 1; i >= 0; i--)
	if (!strcmp(str, functions[i].name)) {
	    *type = functions[i].type;
	    return 3;
	}
    *type = 0;
    return 0;
}

void dump(void)
{
    int i;

    for (i = 0; i < local_idx; i++)
	printf("lname = %s, type = %d\n", locals[i].name, locals[i].type);
    for (i = 0; i < global_idx; i++)
	printf("gname = %s, type = %d\n", globals[i].name, globals[i].type);
    for (i = 0; i < function_idx; i++)
	printf("fname = %s, type = %d\n", functions[i].name, functions[i].type);
}

int main(int argc, char *argv[])
{
    int i, j;
    operator op;

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
	dump();
	return 0;
    }
    fprintf(stderr, "%d line(s)\n", code_idx);
    for (j = 0; operator_NAMES[j]; j++)
	;
    for (i = 0; i < code_idx; i++) {
	printf("%d\t", i);
	if ((op = code[i].op) < j)
	    printf("%-9s", operator_NAMES[op]);
	else
	    printf("%-9c", op);
	if (op < hlt)
	    printf("\t%" PRId64, code[i].val);
	printf("\n");
    }
    return 0;
}
