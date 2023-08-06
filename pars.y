%{
/*
    module  : pars.y
    version : 1.3
    date    : 08/06/23
*/
#include "mcc.h"

int errorcount;
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
	| error { my_error("expected a number", &@1); }
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

/*
    my_error - error processing during parsing; location info as parameter.
*/
void my_error(const char *str, YYLTYPE *bloc)
{
    int leng;

    if (!line[0]) /* only one error per line */
	return;
    fflush(stdout);
    leng = bloc->last_column - 1;
    fprintf(stderr, "%s\n%*s^\n%*s%s\n", line, leng, "", leng, "", str);
    line[0] = 0; /* invalidate line */
    errorcount++;
}

/*
    yyerror - error processing during source file read; location info global.
*/
void yyerror(const char *str)
{
    YYLTYPE bloc;

    if (!strcmp(str, "syntax error"))
	return;
    bloc.last_column = yylloc.last_column;
    my_error(str, &bloc);
}
