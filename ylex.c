/*
    module  : ylex.c
    version : 1.1
    date    : 10/13/23
*/
#include "mcc.h"
#include "kvec.h"

#ifdef TEST
YYSTYPE yylval;
YYLTYPE yylloc;
#endif

typedef int Symbol;	/* symbol created by scanner */

typedef struct Token {
    YYSTYPE yylval;
    Symbol symb;
} Token;

vector(Token) *tokens;	/* read ahead table */

/*
    Push a symbol into the tokenlist.
*/
static void push_symb(Symbol symb)
{
    Token tok;

    if (!tokens)
	vec_init(tokens);
    tok.symb = symb;
    tok.yylval = yylval;
    vec_push(tokens, tok);
}

/*
    yylex - wrapper around my_yylex, storing tokens read, reading from the
	    store or just calling my_yylex itself. This allows tokens to be
	    read twice.

    Read all tokens upto ';' and include them in the tokenlist.
*/
int yylex(void)
{
    Token tok;
    Symbol symb;

/*
    If there is a tokenlist, extract tokens from there.
*/
start:
    if (vec_size(tokens)) {
	tok = vec_pop(tokens);
	symb = tok.symb;
	yylval = tok.yylval;
	return symb;
    }
/*
    There is no tokenlist, use the normal function to get all tokens up to ';'.
*/
    while ((symb = my_yylex()) != ';' && symb) {
	tok.symb = symb;
	tok.yylval = yylval;
	push_symb(symb);
    }
    tok.symb = symb;		/* ; */
    tok.yylval = yylval;
    push_symb(symb);		/* store the last symbol that was read */
    push_symb(symb);		/* extra sym for the benefit of reverse */
    vec_reverse(tokens);
    goto start;
}

/*
    next_symb is used to look at the next symbol, without reading it.
*/
int next_symb(void)
{
    Token tok;

    tok = vec_back(tokens);
    return tok.symb;
}

#ifdef TEST
int main()
{
    Symbol symb;

    while ((symb = yylex()) != 0)
	printf("%d\n", symb);
}
#endif
