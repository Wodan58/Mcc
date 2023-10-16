%{
/*
    module  : lexr.l
    version : 1.12
    date    : 10/13/23
*/
#include "mcc.h"

#define YY_DECL		int my_yylex(void)

char line[MAXLIN + 1]; /* input line */

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

/* Update the column positions, each time some text is matched. */
#define YY_USER_ACTION	{ yylloc.first_column = yylloc.last_column; \
			  yylloc.last_column += yyleng; }
#define YY_USER_INIT	ungetc('\n', stdin)
%}

%option yylineno

D			[0-9]
L			[a-zA-Z_]
A			[a-zA-Z_0-9]

%%

<<EOF>>			return 0;
[\n]+.*\n		{ int i = copyline(); yyless(i); }
"<="			{ return LE_OP; }
">="			{ return GE_OP; }
"=="			{ return EQ_OP; }
"!="			{ return NE_OP; }
"if"			{ return IF; }
"else"			{ return ELSE; }
"endif"			{ return ENDIF; }
"for"			{ return FOR; }
"while"			{ return WHILE; }
"int"			{ return INT; }
"return"		{ return RETURN; }
{D}+			{ yylval.num = strtoll(yytext, 0, 0); return CONSTANT; }
{L}{A}*			{ yylval.str = GC_strdup(yytext); return IDENTIFIER; }
[ \b\t\n\v\f\r]		;
.			return yytext[0];

%%

int yywrap(void)	/* called at end of input */
{
    return 1;		/* terminate now */
}
