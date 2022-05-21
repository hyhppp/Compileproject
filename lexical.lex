%option yylineno
%{
    #include <string.h>
    #include "ast.h"
    #include "grammar.hpp"
    int yycolumn=1;
    extern int mistake;
%}


alphabet [a-zA-Z]
number  [0-9]
numbers [0-9]+

LB (
RB )
LMB [
RMB ]
LBB {
RBB }
LETH <
MOTH >
LETHE <=
MOTHE >=
ISE ==
NOTE !=



PLUS \+
MINUS \-
STAR \*
DIVISION \/

COMMA ,
SEMICOLON ;
ASSIGN =
AND &&
OR ||
NOT !

FOR for
WHILE while
BREAK break
IF if
ELSE else
RETURN return

INT 0|[1-9]{number}*
DOUBLE {numbers}\.{numbers}
CHAR  \'.\'
STRING \"(\\.)*\"
BOOL "true"|"false"
TYPE "int"|"double"|"char"|"string"|"boolean"

CR \r
LF \n
TAB " "

COMMENT \/\/[^\n]*


%%

{LB}   {yylval.token_tree=new Node(yytext,"LB",0);return LB;}
{RB}   {yylval.token_tree=new Node(yytext,"RB",0);return RB;}
{LMB}   {yylval.token_tree=new Node(yytext,"LMB",0);return LMB;}
{RMB}   {yylval.token_tree=new Node(yytext,"RMB",0);return RMB;}
{LBB}   {yylval.token_tree=new Node(yytext,"LBB",0);return LBB;}
{RBB}   {yylval.token_tree=new Node(yytext,"RBB",0);return RBB;}
{LETH}   {yylval.token_tree=new Node(yytext,"LETH",0);return LETH;}
{MOTH}   {yylval.token_tree=new Node(yytext,"MOTH",0);return MOTH;}
{LETHE}   {yylval.token_tree=new Node(yytext,"LETHE",0);return LETHE;}
{MOTHE}   {yylval.token_tree=new Node(yytext,"MOTHE",0);return MOTHE;}
{ISE}   {yylval.token_tree=new Node(yytext,"ISE",0);return ISE;}
{NOTE}   {yylval.token_tree=new Node(yytext,"NOTE",0);return NOTE;}


{PLUS}   {yylval.token_tree=new Node(yytext,"PLUS",0);return PLUS;}
{MINUS}   {yylval.token_tree=new Node(yytext,"MINUS",0);return MINUS;}
{STAR}   {yylval.token_tree=new Node(yytext,"STAR",0);return STAR;}
{DIVISION}   {yylval.token_tree=new Node(yytext,"DIVISION",0);return DIVISION;}

{COMMA}   {yylval.token_tree=new Node(yytext,"COMMA",0);return COMMA;}
{SEMICOLON}   {yylval.token_tree=new Node(yytext,"SEMICOLON",0);return SEMICOLON;}
{ASSIGN}   {yylval.token_tree=new Node(yytext,"ASSIGN",0);return ASSIGN;}
{AND}   {yylval.token_tree=new Node(yytext,"AND",0);return AND;}
{OR}   {yylval.token_tree=new Node(yytext,"OR",0);return OR;}
{NOT}   {yylval.token_tree=new Node(yytext,"NOT",0);return NOT;}

{FOR}   {yylval.token_tree=new Node(yytext,"FOR",0);return FOR;}
{WHILE}   {yylval.token_tree=new Node(yytext,"WHILE",0);return WHILE;}
{BREAK}   {yylval.token_tree=new Node(yytext,"BREAK",0);return BREAK;}
{IF}   {yylval.token_tree=new Node(yytext,"IF",0);return IF;}
{ELSE}   {yylval.token_tree=new Node(yytext,"ELSE",0);return ELSE;}
{RETURN}   {yylval.token_tree=new Node(yytext,"RETURN",0);return RETURN;}

{INT}   {yylval.token_tree=new Node(yytext,"INT",0);return INT;}
{DOUBLE}   {yylval.token_tree=new Node(yytext,"DOUBLE",0);return DOUBLE;}
{CHAR}   {yylval.token_tree=new Node(yytext,"CHAR",0);return CHAR;}
{STRING}   {yylval.token_tree=new Node(yytext,"STRING",0);return STRING;}
{BOOL}   {yylval.token_tree=new Node(yytext,"BOOL",0);return BOOL;}
{TYPE}   {yylval.token_tree=new Node(yytext,"TYPE",0);return TYPE;}

{CR}   {yylval.token_tree=new Node(yytext,"CR",0);return CR;}
{LF}   {yylval.token_tree=new Node(yytext,"LF",0);return LF;}
{TAB}   {yylval.token_tree=new Node(yytext,"TAB",0);return TAB;}

{COMMENT}   {}

.   {
        mistake++;
        printf("Error Occurs at Line %d: Unknown characters \'%s\'\n", yylineno, yytext);
    }


%%

int yywrap(void) {
    return 1;
}
