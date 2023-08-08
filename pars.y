%{
/*
    module  : pars.y
    version : 1.5
    date    : 08/08/23
*/
#include "mcc.h"

int errorcount;
%}

%token <num> CONSTANT
%token LE_OP
%token GE_OP
%token EQ_OP
%token NE_OP

%type <num> primary_expression
%type <num> unary_expression
%type <num> multiplicative_expression
%type <num> additive_expression
%type <num> relational_expression
%type <num> equality_expression
%type <num> expression

/* generate include file with symbols and types */
%defines

/* advanced semantic type */
%union {
    int64_t num;
};

/* start the grammar with expression */
%start expression

%%

primary_expression
	: CONSTANT
	| '(' additive_expression ')' { $$ = $2; }
	| error { my_error("expected a number", &@1); }
	;

unary_expression
	: primary_expression
	| '+' unary_expression { $$ = $2; }
	| '-' unary_expression { $$ = -$2; }
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression { $$ = $1 * $3; }
	| multiplicative_expression '/' unary_expression { $$ = $1 / $3; }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression { $$ = $1 + $3; }
	| additive_expression '-' multiplicative_expression { $$ = $1 - $3; }
	;

relational_expression
	: additive_expression
	| relational_expression '<' additive_expression { $$ = $1 < $3; }
	| relational_expression '>' additive_expression { $$ = $1 > $3; }
	| relational_expression LE_OP additive_expression { $$ = $1 <= $3; }
	| relational_expression GE_OP additive_expression { $$ = $1 >= $3; }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression { $$ = $1 == $3; }
	| equality_expression NE_OP relational_expression { $$ = $1 != $3; }
	;

expression
	: equality_expression { enterprog(loadimmed, 0, $1); }
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
