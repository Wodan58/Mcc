/*
    module  : name.c
    version : 1.7
    date    : 10/30/23
*/
char *operator_NAMES[] = {
    "LOADIMMED",	/* load immediate */
    "LOADOFFSET",	/* load immediate offset */
    "LOCALADR",		/* local address */
    "GLOBLADR",		/* global address */
    "JMP",		/* unconditional jump */
    "JZ",		/* jump if zero */
    "JNZ",		/* jump if not zero */
    "CALL",		/* function call */
    "ENTER",		/* function entry */
    "HALT",		/* halt */
    "LEAVE",		/* function leave */
    "LOADLOCAL",	/* load integer from stack */
    "LOADGLOBL",	/* load global integer */
    "LOADADR",		/* load address */
    "STORLOCAL",	/* store integer on stack */
    "STORGLOBL",	/* store global integer */
    "STORADR",		/* store value at address */
    "PUSH",		/* push value on stack */
    "PUSHADR",		/* push address on stack */
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
    "!",
    "DIV",
    "WRITEBOOL",
    "WRITEINT",
    "MOD",
    "&",
    "SUB_ADR",
    0
};
