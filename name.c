/*
    module  : name.c
    version : 1.5
    date    : 10/23/23
*/
char *operator_NAMES[] = {
    "LOADIMMED",	/* load immediate */
    "LOCALADR",		/* immediate local address */
    "GLOBLADR",		/* immediate global address */
    "JMP",		/* unconditional jump */
    "JZ",		/* jump if zero */
    "JNZ",		/* jump if not zero */
    "CALL",		/* function call */
    "ENTER",		/* function entry */
    "HALT",		/* halt */
    "LEAVE",		/* function leave */
    "LOADLOCAL",	/* load integer from stack */
    "LOADGLOBL",	/* load global integer */
    "STORLOCAL",	/* store integer on stack */
    "STORGLOBL",	/* store global integer */
    "PUSH",		/* push value on stack */
    "OR",
    "XOR",
    "AND",
    "EQL",
    "NEQ",
    "LSS",
    "GTR",
    "LEQ",
    "GEQ",
    "SHL",
    "SHR",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "WRITEBOOL",
    "WRITEINT",		/* 32 */
    0
};
