%{
/*
    module  : lexr.l
    version : 1.1
    date    : 07/19/23
*/
#include "mcc.h"
#include "pars.h"
%}

D			[0-9]

%%

{D}+			{ yylval = atoi(yytext); return(CONSTANT); }
.			{ /* ignore bad characters */ }

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}
