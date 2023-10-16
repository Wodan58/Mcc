%{
/*
    module  : pars.y
    version : 1.14
    date    : 10/13/23
*/
#include "mcc.h"

struct opcode temp;
int errorcount, regno;
%}

%token <str> IDENTIFIER
%token <num> CONSTANT
%token LE_OP
%token GE_OP
%token EQ_OP
%token NE_OP
%token IF
%token ELSE
%token ENDIF
%token FOR
%token WHILE
%token INT
%token RETURN

%type <op> postfix_expression argument_expression_list
%type <op> primary_expression unary_expression multiplicative_expression
%type <op> additive_expression relational_expression equality_expression
%type <op> assignment_expression expression compound_statement block_item_list
%type <op> block_item expression_statement jump_statement selection_statement
%type <op> iteration_statement statement direct_declarator
%type <op> init_declarator init_declarator_list type_specifier declarator
%type <op> declaration_specifiers declaration external_declaration
%type <op> translation_unit declaration_statement initializer
%type <op> function_definition

%type <num> jiz_block jmp_block jmp_target

/* generate include file with symbols and types */
%defines

/* advanced semantic type */
%union {
    char *str;		/* return values from lexer */
    int64_t num;
    struct opcode {	/* evaluation values parser */
	int regno;
	int index;
    } op;
};

/* start the grammar with translation_unit */
%start translation_unit

%%

primary_expression
	: IDENTIFIER
	  { int index, found, type;
	    index = lookup(yylval.str, &found, &type); /* get address */
	    if (found == -1)
		my_error("var/fun not found", &@1); /* undeclared var/fun */
	    else if (found == 1)
		enterprog(loadlocal, regno, index); /* load address in regno */
	    else if (found == 2)
		enterprog(cal, type, 0); /* number of parameters needed */
#if 0
	    else if (found == 0)
		enterprog(loadglobl, regno, index);
#endif
	  temp.regno = regno++; /* variables are loaded in register regno */
	  temp.index = index; $$ = temp; }
	| CONSTANT { enterprog(loadimmed, 10, yylval.num);
	  temp.regno = 10; /* constants are loaded in register 10 */
	  temp.index = 0; $$ = temp; }
	| '(' additive_expression ')' { $$ = $2; }
	| error { my_error("expected a number", &@1); $$.regno = -1; }
	;

postfix_expression
	: primary_expression
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| '+' unary_expression { $$ = $2; }
	| '-' unary_expression
	  { if (code[code_idx].op == loadimmed)
		code[code_idx].adr2 = -code[code_idx].adr2;
	    else
		enterprog(neg, $2.regno, 0); $$ = $2; }
	;

/* cast expression */

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 *= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(mul, $1.regno, $3.regno); }
	| multiplicative_expression '/' unary_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 /= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(dvd, $1.regno, $3.regno); }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 += code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(add, $1.regno, $3.regno); }
	| additive_expression '-' multiplicative_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 -= code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(sub, $1.regno, $3.regno); }
	;

/* shift expression */

relational_expression
	: additive_expression
	| relational_expression '<' additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 <
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(lss, $1.regno, $3.regno); }
	| relational_expression '>' additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 >
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(gtr, $1.regno, $3.regno); }
	| relational_expression LE_OP additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 <=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(leq, $1.regno, $3.regno); }
	| relational_expression GE_OP additive_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 >=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(geq, $1.regno, $3.regno); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 ==
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(eql, $1.regno, $3.regno); }
	| equality_expression NE_OP relational_expression
	  { if (code[code_idx].op == loadimmed &&
		code[code_idx - 1].op == loadimmed) {
		code[code_idx - 1].adr2 = code[code_idx - 1].adr2 !=
					  code[code_idx].adr2;
		code_idx--;
	    } else
		enterprog(neq, $1.regno, $3.regno); }
	;

/* and expression */

/* or expression */

/* exclusive or expression */

/* inclusive or expression */

/* logical and expression */

/* logical or expression */

/* conditional expression */

assignment_expression
	: equality_expression
	| unary_expression '=' assignment_expression
	  { enterprog(storlocal, $1.index, $3.regno); $$ = $3; }
	;

expression
	: assignment_expression
	| expression ',' assignment_expression { $$ = $3; }
	;

/* constant expression */

block_item
	: statement { $$.regno = 0; }
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

compound_statement
	: '{' '}' { $$.regno = 0; }
	| '{'  block_item_list '}' { $$ = $2; }
	;

expression_statement
	: ';' { $$.regno = regno = 0; }
	| expression ';' { regno = 0; }
	;

jiz_block
	: { enterprog(jiz, -1, code[code_idx].adr1); $$ = code_idx; }
	;

jmp_block
	: { enterprog(jmp, -1, 0); $$ = code_idx; }
	;

selection_statement
	: IF '(' expression ')' jiz_block statement jmp_block
	  { code[$5].adr1 = code_idx + 1; } ELSE statement
	  { code[$7].adr1 = code_idx + 1; } ENDIF { $$.regno = -1; }
	| IF '(' expression ')' jiz_block statement
	  { code[$5].adr1 = code_idx + 1; } ENDIF { $$.regno = -1; }
	;

jmp_target
	: { $$ = code_idx + 1; }
	;

iteration_statement
	: WHILE '(' jmp_target expression jiz_block ')' statement
	  { enterprog(jmp, $3, 0); code[$5].adr1 = code_idx + 1; }
	| FOR '(' expression_statement jmp_target expression_statement
	  jiz_block ')' statement { enterprog(jmp, $4, 0);
	  code[$6].adr1 = code_idx + 1; }
	| FOR '(' expression_statement jmp_target expression_statement
	  jiz_block jmp_block expression { enterprog(jmp, $4, 0);
	  code[$7].adr1 = code_idx + 1; } ')' statement
	  { enterprog(jmp, $7 + 1, 0); code[$6].adr1 = code_idx + 1; }
	;

jump_statement
	: RETURN ';' { $$.regno = regno = 0; }
	| RETURN expression ';' { regno = 0; $$ = $2; }
	;

declaration_statement
	: declaration
	;

statement
	: compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| declaration_statement
	;

initializer
	: assignment_expression
	;

direct_declarator
	: IDENTIFIER
	  { int index, found, type;
	    index = lookup(yylval.str, &found, &type); /* get address */
	    if (found == 1)
		my_error("name already declared", &@1);
	    else if (found == -1) {
		if (next_symb() == '(')
		    enterfunction(code_idx + 1);
		else
		    enterlocal(type); /* enter local into symbol table */
	    }
	  index = lookup(yylval.str, &found, &type); /* get address */
	  temp.regno = type; /* the type of the variable in the regno field */
	  temp.index = index; $$ = temp; }
	| direct_declarator '(' ')'
	;

declarator
	: direct_declarator
	;

init_declarator
	: declarator '=' initializer
	  { enterprog(storlocal, $1.index, $3.regno); }
	| declarator
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

type_specifier
	: INT { temp.regno = temp.index = 0; $$ = temp; }
	;

declaration_specifiers
	: type_specifier
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	  { enterprog(ret, 0, 0); }
	| declaration_specifiers declarator compound_statement
	  { enterprog(ret, 0, 0); }
	;

external_declaration
	: function_definition
	| declaration
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
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
