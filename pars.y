%{
/*
    module  : pars.y
    version : 1.8
    date    : 08/15/23
*/
#include "mcc.h"

int errorcount, regnum, found, type;
%}

%token <str> IDENTIFIER
%token <num> CONSTANT
%token LE_OP
%token GE_OP
%token EQ_OP
%token NE_OP
%token RETURN

%type <num> primary_expression unary_expression

/* generate include file with symbols and types */
%defines

/* advanced semantic type */
%union {
    char *str;
    int64_t num;
};

/* start the grammar with statement_list */
%start statement_list

%%

primary_expression
	: IDENTIFIER
	  { int index = lookup(yylval.str, &found, &type); /* get address */
	    if (found == -1) {
		enterlocal(type);
		index = lookup(yylval.str, &found, &type); /* get address */
	    }
#if 0
		my_error("variable not found", &@1); /* undeclared variable */
	    else if (found == 1)
		enterprog(loadlocal, regnum, index); /* load address in reg */
	    else if (found == 0)
		enterprog(loadglobl, regnum, index);
#endif
	    $$ = index; }
	| CONSTANT { enterprog(loadimmed, regnum, yylval.num); $$ = 0; }
	| '(' additive_expression ')' { $$ = 0; }
	| error { my_error("expected a number", &@1); }
	;

unary_expression
	: primary_expression
	| '+' unary_expression { $$ = $2; }
	| '-' unary_expression
	  { if (code[code_idx].op == loadimmed)
		code[code_idx].adr2 = -code[code_idx].adr2;
	    else
		enterprog(neg, regnum, 0); $$ = $2; }
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' { regnum++; } unary_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 *= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(mul, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| multiplicative_expression '/' { regnum++; } unary_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 /= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(dvd, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' { regnum++; } multiplicative_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 += code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(add, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| additive_expression '-' { regnum++; } multiplicative_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 -= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(sub, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	;

relational_expression
	: additive_expression
	| relational_expression '<' { regnum++; } additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 <
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(lss, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| relational_expression '>' { regnum++; } additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 >
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(gtr, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| relational_expression LE_OP { regnum++; } additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 <=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(leq, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| relational_expression GE_OP { regnum++; } additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 >=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(geq, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP { regnum++; } relational_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 ==
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(eql, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	| equality_expression NE_OP { regnum++; } relational_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 !=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(neq, regnum - 1, regnum);
	    regnum--; } /* discard register from second factor */
	;

assignment_expression
	: equality_expression
	| unary_expression '=' assignment_expression
	  { enterprog(storlocal, $1, regnum); }
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

expression_statement
	: ';'
	| expression ';'
	;

jump_statement
	: RETURN ';'
	| RETURN expression ';'
	;

statement
	: expression_statement
	| jump_statement
	;

statement_list
	: statement
	| statement_list statement
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
