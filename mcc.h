/*
    module  : mcc.h
    version : 1.4
    date    : 08/17/23
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
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
    add,
    sub,
    mul,
    dvd,
    eql,
    neq,
    gtr,
    geq,
    lss,
    leq,
    neg,
    loadlocal,
    loadimmed,
    storlocal,
    jmp,
    jiz
} operator;


typedef struct instruction {
    operator op;
    int64_t adr1, adr2;
} instruction;

/* --------------------------- V A R I A B L E S --------------------------- */

extern char line[];
extern instruction code[];
extern char *operator_NAMES[];
extern int code_idx, errorcount;

/* --------------------------- F U N C T I O N S --------------------------- */

int yylex(void);
void my_error(const char *str, YYLTYPE *bloc);
void yyerror(const char *str); /* prints grammar violation message */
void enterprog(operator op, int64_t adr1, int64_t adr2);
void enterlocal(int type);
int lookup(char *str, int *found, int *type);
