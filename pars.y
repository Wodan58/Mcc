%{
/*
    module  : pars.y
    version : 1.16
    date    : 10/27/23
*/
#include "mcc.h"

int errorcount;
%}

%token <str> IDENTIFIER
%token <num> I_CONSTANT

%token F_CONSTANT STRING_LITERAL FUNC_NAME SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN
%token TYPEDEF_NAME ENUMERATION_CONSTANT

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE
%token CONST RESTRICT VOLATILE
%token BOOL CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token ALIGNAS ALIGNOF ATOMIC GENERIC NORETURN STATIC_ASSERT THREAD_LOCAL

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%token WRITEBOOL WRITEINT

%type <num> unary_operator assign jiz_block jmp_block jmp_target

/* generate include file with symbols and types */
%defines

/* advanced semantic type */
%union {
    char *str;		/* return values from lexer */
    int64_t num;
};

/* start the grammar with translation_unit */
%start translation_unit

%%

primary_expression
	: IDENTIFIER { int64_t val; int found = lookup($1, &val); if (!found)
	  my_error("name not found", &@1); else if (found == 3)
	  enterprog(cal, val); else { enterprog(loadimmed, val); if (found == 1)
	  enterprog(loadlocal, 0); else enterprog(loadglobl, 0); } }
	| WRITEBOOL { enterprog(writebool, 0); }
	| WRITEINT { enterprog(writeint, 0); }
	| constant
	| string
	| '(' expression ')'
	| generic_selection
	;

constant
	: I_CONSTANT { enterprog(loadimmed, $1); }
				/* includes character_constant */
	| F_CONSTANT
	| ENUMERATION_CONSTANT	/* after it has been defined as such */
	;

enumeration_constant		/* before it has been defined as such */
	: IDENTIFIER
	;

string
	: STRING_LITERAL
	| FUNC_NAME
	;

generic_selection
	: GENERIC '(' assignment_expression ',' generic_assoc_list ')'
	;

generic_assoc_list
	: generic_association
	| generic_assoc_list ',' generic_association
	;

generic_association
	: type_name ':' assignment_expression
	| DEFAULT ':' assignment_expression
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression { if ($1 != '+') enterprog($1, 0); }
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	| ALIGNOF '(' type_name ')'
	;

unary_operator
	: '&' { $$ = '&'; }
	| '*' { $$ = '*'; }
	| '+' { $$ = '+'; }
	| '-' { $$ = '-'; }
	| '~' { $$ = '~'; }
	| '!' { $$ = '!'; }
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' { enterprog(push, 0); } cast_expression
	  { enterprog(mul, 0); }
	| multiplicative_expression '/' { enterprog(push, 0); } cast_expression
	  { enterprog(dvd, 0); }
	| multiplicative_expression '%' { enterprog(push, 0); } cast_expression
	  { enterprog(mod, 0); }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' { enterprog(push, 0); }
	  multiplicative_expression { enterprog(add, 0); }
	| additive_expression '-' { enterprog(push, 0); }
	  multiplicative_expression { enterprog(sub, 0); }
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' { enterprog(push, 0); } shift_expression
	  { enterprog(lss, 0); }
	| relational_expression '>' { enterprog(push, 0); } shift_expression
	  { enterprog(gtr, 0); }
	| relational_expression LE_OP { enterprog(push, 0); } shift_expression
	  { enterprog(leq, 0); }
	| relational_expression GE_OP { enterprog(push, 0); } shift_expression
	  { enterprog(geq, 0); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP { enterprog(push, 0); }
	  relational_expression { enterprog(eql, 0); }
	| equality_expression NE_OP { enterprog(push, 0); }
	  relational_expression { enterprog(neq, 0); }
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP { enterprog(push, 0); }
	  inclusive_or_expression { enterprog(bit_and, 0); }
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP { enterprog(push, 0); }
	  logical_and_expression { enterprog(bit_or, 0); }
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assign
	: { $$ = assign(); }
	;

assignment_expression
	: conditional_expression
	| unary_expression assign assignment_operator assignment_expression
	  { enterprog($2 == '*' ? storadr : ($2 == loadlocal ? storlocal :
	    storglobl), 0); }
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression	/* with constraints */
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	| static_assert_declaration
	;

declaration_specifiers
	: storage_class_specifier declaration_specifiers
	| storage_class_specifier
	| type_specifier declaration_specifiers
	| type_specifier
	| type_qualifier declaration_specifiers
	| type_qualifier
	| function_specifier declaration_specifiers
	| function_specifier
	| alignment_specifier declaration_specifiers
	| alignment_specifier
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator '=' initializer
	| declarator
	;

storage_class_specifier
	: TYPEDEF	/* identifiers must be flagged as TYPEDEF_NAME */
	| EXTERN
	| STATIC
	| THREAD_LOCAL
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT
	| INT
	| LONG
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
	| BOOL
	| COMPLEX
	| IMAGINARY	  	/* non-mandated extension */
	| atomic_type_specifier
	| struct_or_union_specifier
	| enum_specifier
	| TYPEDEF_NAME		/* after it has been defined as such */
	;

struct_or_union_specifier
	: struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list ';'	/* for anonymous struct/union */
	| specifier_qualifier_list struct_declarator_list ';'
	| static_assert_declaration
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: ':' constant_expression
	| declarator ':' constant_expression
	| declarator
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator	/* identifiers must be flagged as ENUMERATION_CONSTANT */
	: enumeration_constant '=' constant_expression
	| enumeration_constant
	;

atomic_type_specifier
	: ATOMIC '(' type_name ')'
	;

type_qualifier
	: CONST
	| RESTRICT
	| VOLATILE
/*	| ATOMIC	*/
	;

function_specifier
	: INLINE
	| NORETURN
	;

alignment_specifier
	: ALIGNAS '(' type_name ')'
	| ALIGNAS '(' constant_expression ')'
	;

declarator
	: pointer direct_declarator
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER { int64_t value; int found = lookup($1, &value);
	  if (!found) { if (next_symb() == '(') enterfunction(); else
	  enterlocal(); } else my_error("name already declared", &@1); }
	| '(' declarator ')'
	| direct_declarator '[' ']'
	| direct_declarator '[' '*' ']'
	| direct_declarator '[' STATIC type_qualifier_list
	  assignment_expression ']'
	| direct_declarator '[' STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list '*' ']'
	| direct_declarator '[' type_qualifier_list STATIC
	  assignment_expression ']'
	| direct_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list ']'
	| direct_declarator '[' assignment_expression ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' ')'
	| direct_declarator '(' identifier_list ')'
	;

pointer
	: '*' type_qualifier_list pointer
	| '*' type_qualifier_list
	| '*' pointer
	| '*'
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list ',' ELLIPSIS
	| parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator
	: pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' '*' ']'
	| '[' STATIC type_qualifier_list assignment_expression ']'
	| '[' STATIC assignment_expression ']'
	| '[' type_qualifier_list STATIC assignment_expression ']'
	| '[' type_qualifier_list assignment_expression ']'
	| '[' type_qualifier_list ']'
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' '*' ']'
	| direct_abstract_declarator '[' STATIC type_qualifier_list
	  assignment_expression ']'
	| direct_abstract_declarator '[' STATIC assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list
	  assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list STATIC
	  assignment_expression ']'
	| direct_abstract_declarator '[' type_qualifier_list ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	| assignment_expression
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list ',' designation initializer
	| initializer_list ',' initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
	;

static_assert_declaration
	: STATIC_ASSERT '(' constant_expression ',' STRING_LITERAL ')' ';'
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{'  block_item_list '}'
	;

block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: declaration
	| statement
	;

expression_statement
	: ';'
	| expression ';'
	;

jiz_block
	: { $$ = code_idx; enterprog(jiz, 0); }
	;

jmp_block
	: { $$ = code_idx; enterprog(jmp, 0); }
	;

selection_statement
	: IF '(' expression ')' jiz_block statement
	  ELSE jmp_block { code[$5].val = code_idx; } statement
	  { code[$8].val = code_idx; } 
	| IF '(' expression ')' jiz_block statement %prec LOWER_THAN_ELSE
	  { code[$5].val = code_idx; }
	| SWITCH '(' expression ')' statement
	;


jmp_target
	: { $$ = code_idx; }
	;

iteration_statement
	: WHILE jmp_target '(' expression ')' jiz_block statement
	  { enterprog(jmp, $2); code[$6].val = code_idx; }
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement jmp_target expression_statement ')'
	  jiz_block statement { enterprog(jmp, $4); code[$7].val = code_idx; }
	| FOR '(' expression_statement jmp_target expression_statement
	  jiz_block jmp_block expression { enterprog(jmp, $4); code[$7].val =
	  code_idx; } ')' statement { enterprog(jmp, $7 + 1); code[$6].val =
	  code_idx; }
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: { enterprog(cal, 0); enterprog(hlt, 0); } external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration
	;

function_definition
	: declaration_specifiers declarator { enterprog(ent, 0); }
	  declaration_list compound_statement { enterprog(lev, 0); }
	| declaration_specifiers declarator { enterprog(ent, 0); }
	  compound_statement { enterprog(lev, 0); }
	;

declaration_list
	: declaration
	| declaration_list declaration
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
