%{
/*
    module  : pars.y
    version : 1.1
    date    : 07/19/23
*/
#include "mcc.h"
%}

%token CONSTANT

/* generate include file with symbols and types */
%defines

/* start the grammar with primary_expression */
%start primary_expression

%%

primary_expression
	: CONSTANT { enterprog(loadimmed, 0, yylval); }
	;

%%

void yyerror(const char *str)
{
    fflush(stdout);
    fprintf(stderr, "*** %s\n", str);
}
