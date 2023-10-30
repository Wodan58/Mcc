/*
    module  : mcc.h
    version : 1.9
    date    : 10/30/23
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#ifndef FLEXINT_H
#include <stdint.h>
#endif
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
    loadimmed,	/*  0 load immediate 64-bits */
    loadoffset,	/*  1 load immediate 64-bits offset */
    localadr,	/*  2 local address 64 bits */
    globladr,	/*  3 global address 64 bits */
    jmp,	/*  4 unconditional jump 32-bits */
    jiz,	/*  5 jump if zero 32-bits */
    jnz,	/*  6 jump if not zero 32-bits */
    cal,	/*  7 function call 32-bits */
    ent,	/*  8 function entry 32-bits */
    hlt,	/*  9 halt */
    lev,	/* 10 function leave */
    loadlocal,	/* 11 load integer from stack */
    loadglobl,	/* 12 load global integer */
    loadadr,	/* 13 load address from stack */
    storlocal,	/* 14 store integer on stack */
    storglobl,	/* 15 store global integer */
    storadr,	/* 16 store value at address */
    push,	/* 17 push value on stack */
    pushadr,	/* 18 push address on stack */
    bit_or,	/* 19 bitwise operators */
    bit_xor,	/* 20 */
    bit_and,	/* 21 */
    eql,	/* 22 comparison operators */
    neq,	/* 23 */
    lss,	/* 24 */
    gtr,	/* 25 */
    leq,	/* 26 */
    geq,	/* 27 */
    shl,	/* 28 shuffle operators */
    shr,	/* 29 */
    add,	/* 30 arithmetic operators */
    sub,	/* 31 */
    mul,	/* 32 */
    log_not,	/* !  */
    dvd,	/* "  */
    writebool,	/* #  */
    writeint,	/* $  */
    mod,	/* %  */
    adr_of,	/* &  address of */
    sub_adr,	/* '  */
		/* (  */
		/* )  */
		/* *  dereference */
		/* +  unary plus */
		/* ,  */
		/* -  unary minus */
		/* .  */
		/* /  */
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
