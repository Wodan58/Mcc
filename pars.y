%{
/*
    module  : pars.y
    version : 1.2
    date    : 07/20/23
*/
#include "mcc.h"
%}

%token <num> CONSTANT

%type <num> primary_expression
%type <num> additive_expression
%type <num> expression

/* generate include file with symbols and types */
%defines

/* advanced semantic type */
%union {
    int64_t num;
};

/* start the grammar with primary_expression */
%start expression

%%

primary_expression
	: CONSTANT
	;

additive_expression
	: primary_expression
	| additive_expression '+' primary_expression { $$ = $1 + $3; }
	| additive_expression '-' primary_expression { $$ = $1 - $3; }
	;

expression
	: additive_expression { enterprog(loadimmed, 0, $1); }
	;

%%

void yyerror(const char *str)
{
    fflush(stdout);
    fprintf(stderr, "*** %s\n", str);
}
