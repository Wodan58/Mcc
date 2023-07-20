%{
/*
    module  : lexr.l
    version : 1.2
    date    : 07/20/23
*/
#include "mcc.h"
#include "pars.h"
%}

D			[0-9]

%%

{D}+			{ yylval.num = strtoll(yytext, 0, 10);
			  return(CONSTANT); }
[ \t\v\n\f]		;
.			return yytext[0];

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}
