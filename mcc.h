/*
    module  : mcc.h
    version : 1.1
    date    : 07/19/23
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

/* ----------------------------- D E F I N E S ----------------------------- */

/*
    maximum size of the instruction table
*/
#define MAXPRG	200

typedef enum operator {
    loadimmed
} operator;

/* ------------------------------- T Y P E S ------------------------------- */

typedef struct instruction {
    operator op;
    int64_t adr1, adr2;
} instruction;

/* --------------------------- V A R I A B L E S --------------------------- */

extern int code_idx;
extern instruction code[];
extern char *operator_NAMES[];

/* --------------------------- F U N C T I O N S --------------------------- */

int yylex(void);
void yyerror(const char *str);  /* prints grammar violation message */
void enterprog(operator op, int64_t adr1, int64_t adr2);
