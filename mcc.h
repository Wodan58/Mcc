/*
    module  : mcc.h
    version : 1.7
    date    : 10/23/23
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <gc.h>
#include "pars.h"

/* ----------------------------- D E F I N E S ----------------------------- */

/*
    maximum size of an input line
*/
#define MAXLIN	100

/*
    maximum size of the instruction table
*/
#define MAXPRG	200

/* ------------------------------- T Y P E S ------------------------------- */

typedef enum operator {
    loadimmed,	/* load immediate 64-bits */
    localadr,	/* immediate address 64 bits */
    globladr,	/* immediate address 64 bits */
    jmp,	/* unconditional jump 32-bits */
    jiz,	/* jump if zero 32-bits */
    jnz,	/* jump if not zero 32-bits */
    cal,	/* function call 32-bits */
    ent,	/* function entry 32-bits */
    hlt,	/* halt */
    lev,	/* function leave */
    loadlocal,	/* load integer from stack */
    loadglobl,	/* load global integer */
    storlocal,	/* store integer on stack */
    storglobl,	/* store global integer */
    push,	/* push value on stack */
    orr,
    xrr,
    ann,
    eql,
    neq,
    lss,
    gtr,
    leq,
    geq,
    shl,
    shr,
    add,
    sub,
    mul,
    dvd,
    mod,
    writebool,
    writeint	/* 32 */
} operator;

typedef struct instruction {
    unsigned char label;	/* target of jump/call */
    operator op;
    int64_t val;
} instruction;

/* --------------------------- V A R I A B L E S --------------------------- */

extern char line[];
extern instruction code[];
extern char *operator_NAMES[];
extern int code_idx, errorcount;

/* --------------------------- F U N C T I O N S --------------------------- */

int yylex(void);
int next_symb(void);
int my_yylex(void);
void my_error(const char *str, YYLTYPE *bloc);
void yyerror(const char *str);	/* prints grammar violation message */
void enterprog(operator op, int64_t val);
void enterlocal(void);
void enterfunction(void);
int lookup(char *str, int64_t *type);
operator assign(void);
