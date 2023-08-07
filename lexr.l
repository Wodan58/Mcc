%{
/*
    module  : lexr.l
    version : 1.4
    date    : 08/07/23
*/
#include "mcc.h"

char line[MAXLIN + 1];	/* input line */

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

%%

[\n]+.*\n		{ int i = copyline(); yyless(i); }
{D}+			{ yylval.num = strtoll(yytext, 0, 10);
			  return(CONSTANT); }
[ \t\v\n\f]		;
.			return yytext[0];

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}
