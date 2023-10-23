%{
/*
    module  : lexr.l
    version : 1.13
    date    : 10/23/23
*/
#include "mcc.h"

/* ----------------------------- D E F I N E S ----------------------------- */

#define YY_DECL		int my_yylex(void)

/* Update the column positions, each time some text is matched. */
#define YY_USER_ACTION	{ yylloc.first_column = yylloc.last_column; \
			  yylloc.last_column += yyleng; }
#define YY_USER_INIT	ungetc('\n', stdin)

#define sym_type(identifier) IDENTIFIER	/* with no symbol table, fake it */

/* --------------------------- V A R I A B L E S --------------------------- */

char line[MAXLIN + 1];	/* input line */

/* --------------------------- F U N C T I O N S --------------------------- */

void comment(void);
int check_type(void);
int i_constant(void);

int copyline()
{
    int i = 0, j;

    while (yytext[i] == '\n')
	i++;
    strncpy(line, &yytext[i], yyleng - i); /* copy input line */
    if ((j = yyleng - i - 1) < 0)
	j = 0;
    line[j] = 0; /* replace \n by \0 */
    yylloc.first_column = yylloc.last_column = 0; /* reset columns */
    return i;
}
%}

%option yylineno

O	[0-7]
D	[0-9]
NZ	[1-9]
L	[a-zA-Z_]
A	[a-zA-Z_0-9]
H	[a-fA-F0-9]
HP	(0[xX])
E	([Ee][+-]?{D}+)
P	([Pp][+-]?{D}+)
FS	(f|F|l|L)
IS	(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP	(u|U|L)
SP	(u8|u|U|L)
ES	(\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS	[ \b\t\n\v\f\r]

%%

<<EOF>>			return 0;
[\n]+.*\n		{ int i = copyline(); yyless(i); }
"/*"			{ comment(); }
"//".*			{ /* consume //-comment */ }

"auto"			{ return AUTO; }
"break"			{ return BREAK; }
"case"			{ return CASE; }
"char"			{ return CHAR; }
"const"			{ return CONST; }
"continue"		{ return CONTINUE; }
"default"		{ return DEFAULT; }
"do"			{ return DO; }
"double"		{ return DOUBLE; }
"else"			{ return ELSE; }
"enum"			{ return ENUM; }
"extern"		{ return EXTERN; }
"float"			{ return FLOAT; }
"for"			{ return FOR; }
"goto"			{ return GOTO; }
"if"			{ return IF; }
"inline"		{ return INLINE; }
"int"			{ return INT; }
"long"			{ return LONG; }
"register"		{ return REGISTER; }
"restrict"		{ return RESTRICT; }
"return"		{ return RETURN; }
"short"			{ return SHORT; }
"signed"		{ return SIGNED; }
"sizeof"		{ return SIZEOF; }
"static"		{ return STATIC; }
"struct"		{ return STRUCT; }
"switch"		{ return SWITCH; }
"typedef"		{ return TYPEDEF; }
"union"			{ return UNION; }
"unsigned"		{ return UNSIGNED; }
"void"			{ return VOID; }
"volatile"		{ return VOLATILE; }
"while"			{ return WHILE; }
"_Alignas"		{ return ALIGNAS; }
"_Alignof"		{ return ALIGNOF; }
"_Atomic"		{ return ATOMIC; }
"_Bool"			{ return BOOL; }
"_Complex"		{ return COMPLEX; }
"_Generic"		{ return GENERIC; }
"_Imaginary"		{ return IMAGINARY; }
"_Noreturn"		{ return NORETURN; }
"_Static_assert"	{ return STATIC_ASSERT; }
"_Thread_local"		{ return THREAD_LOCAL; }
"__func__"		{ return FUNC_NAME; }

"writebool"		{ return WRITEBOOL; }
"writeint"		{ return WRITEINT; }

{L}{A}*			{ return check_type(); }

{HP}{H}+{IS}?				{ return i_constant(); }
{NZ}{D}*{IS}?				{ return i_constant(); }
"0"{O}*{IS}?				{ return i_constant(); }
{CP}?"'"([^'\\\n]|{ES})+"'"		{ return i_constant(); }

{D}+{E}{FS}?				{ return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ return F_CONSTANT; }
{D}+"."{E}?{FS}?			{ return F_CONSTANT; }
{HP}{H}+{P}{FS}?			{ return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ return F_CONSTANT; }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ return STRING_LITERAL; }

"..."			{ return ELLIPSIS; }
">>="			{ return RIGHT_ASSIGN; }
"<<="			{ return LEFT_ASSIGN; }
"+="			{ return ADD_ASSIGN; }
"-="			{ return SUB_ASSIGN; }
"*="			{ return MUL_ASSIGN; }
"/="			{ return DIV_ASSIGN; }
"%="			{ return MOD_ASSIGN; }
"&="			{ return AND_ASSIGN; }
"^="			{ return XOR_ASSIGN; }
"|="			{ return OR_ASSIGN; }
">>"			{ return RIGHT_OP; }
"<<"			{ return LEFT_OP; }
"++"			{ return INC_OP; }
"--"			{ return DEC_OP; }
"->"			{ return PTR_OP; }
"&&"			{ return AND_OP; }
"||"			{ return OR_OP; }
"<="			{ return LE_OP; }
">="			{ return GE_OP; }
"=="			{ return EQ_OP; }
"!="			{ return NE_OP; }
";"			{ return ';'; }
("{"|"<%")		{ return '{'; }
("}"|"%>")		{ return '}'; }
","			{ return ','; }
":"			{ return ':'; }
"="			{ return '='; }
"("			{ return '('; }
")"			{ return ')'; }
("["|"<:")		{ return '['; }
("]"|":>")		{ return ']'; }
"."			{ return '.'; }
"&"			{ return '&'; }
"!"			{ return '!'; }
"~"			{ return '~'; }
"-"			{ return '-'; }
"+"			{ return '+'; }
"*"			{ return '*'; }
"/"			{ return '/'; }
"%"			{ return '%'; }
"<"			{ return '<'; }
">"			{ return '>'; }
"^"			{ return '^'; }
"|"			{ return '|'; }
"?"			{ return '?'; }

{WS}+			{ /* whitespace separates tokens */ }
.			{ /* discard bad characters */ }

%%

int yywrap(void)	/* called at end of input */
{
    return 1;		/* terminate now */
}

void comment(void)
{
    int c;

    while ((c = input()) != 0)
        if (c == '*') {
            while ((c = input()) == '*')
                ;
            if (c == '/')
                return;
            if (c == 0)
                break;
        }
    yyerror("unterminated comment");
}

int check_type(void)
{
    switch (sym_type(yytext)) {
    case TYPEDEF_NAME:                /* previously defined */
	return TYPEDEF_NAME;
    case ENUMERATION_CONSTANT:        /* previously defined */
	return ENUMERATION_CONSTANT;
    default:                          /* includes undefined */
	yylval.str = GC_strdup(yytext);
	return IDENTIFIER;
    }
}

int i_constant(void)
{
    yylval.num = strtoll(yytext, 0, 0);
    return I_CONSTANT;
}
