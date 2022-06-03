
%option yylineno
%{
    #include <string.h>
    #include "ast.h"
    #include "grammar.hpp"
    int yycolumn=1;
    #define YY_USER_ACTION \
        yylloc.first_line=yylineno;\
        yylloc.last_line=yylineno;\
        yylloc.first_column=yycolumn;\
        yylloc.last_column=yylloc.first_column+yyleng-1;\
        yycolumn+=yyleng;
    extern int mistakeRecord[4096];
    extern int mistake;
%}


alphabet [a-zA-Z]
number  [0-9]
numbers [0-9]+

LP \(
RP \)
LMB \[
RMB \]
LC \{
RC \}
LETH <
MOTH >
LETHE <=
MOTHE >=
ISE ==
NOTE !=

NAME [a-zA-Z_][0-9a-zA-Z_]*



PLUS \+
MINUS \-
STAR \*
DIVISION \/

COMMA ,
SEMICOLON ;
ASSIGN =
AND &&
OR "||"
NOT !

WHILE while
BREAK break
IF if
ELSE else
RETURN return

INT 0|[1-9]{number}*
FLOAT {numbers}\.{numbers}
CHAR  \'.\'|\'\\.\'
STRING \"(\\.|[^"\\])*\"
BOOL "true"|"false"
TYPE "int"|"float"|"char"|"string"|"boolean"

CR \r
LF \n
TAB \t|" "

COMMENTMULTILINE \/\*([^\*]|(\*)*[^\*/])*(\*)*\*\/ 
COMMENTLINE \/\/[^\n]*


%%

{LP}   {yylval.token_tree = new Node(yytext, "LP"); return LP;}
{RP}   {yylval.token_tree = new Node(yytext, "RP"); return RP;}
{LMB}   {yylval.token_tree = new Node(yytext, "LMB"); return LMB;}
{RMB}   {yylval.token_tree = new Node(yytext, "RMB"); return RMB;}
{LC}   {yylval.token_tree = new Node(yytext, "LC"); return LC;}
{RC}   {yylval.token_tree = new Node(yytext, "RC"); return RC;}
{LETH}   {yylval.token_tree=new Node(yytext,"LETH");return LETH;}
{MOTH}   {yylval.token_tree=new Node(yytext,"MOTH");return MOTH;}
{LETHE}   {yylval.token_tree=new Node(yytext,"LETHE");return LETHE;}
{MOTHE}   {yylval.token_tree=new Node(yytext,"MOTHE");return MOTHE;}
{ISE}   {yylval.token_tree=new Node(yytext,"ISE");return ISE;}
{NOTE}   {yylval.token_tree=new Node(yytext,"NOTE");return NOTE;}



{PLUS}  {yylval.token_tree = new Node(yytext, "PLUS"); return PLUS;}
{MINUS}   {yylval.token_tree = new Node(yytext, "MINUS"); return MINUS;}
{STAR}   {yylval.token_tree = new Node(yytext, "STAR"); return STAR;}
{DIVISION}   {yylval.token_tree = new Node(yytext, "DIVISION"); return DIVISION;}

{COMMA}   {yylval.token_tree = new Node(yytext, "COMMA"); return COMMA;}
{SEMICOLON}   {yylval.token_tree = new Node(yytext, "SEMICOLON"); return SEMICOLON;}
{ASSIGN}   {yylval.token_tree = new Node(yytext, "ASSIGN"); return ASSIGN;}
{AND}   {yylval.token_tree = new Node(yytext, "AND"); return AND;}
{OR}   {yylval.token_tree = new Node(yytext, "OR"); return OR;}
{NOT}   {yylval.token_tree = new Node(yytext, "NOT"); return NOT;}

{WHILE}   {yylval.token_tree = new Node(yytext, "WHILE"); return WHILE;}
{BREAK}   {yylval.token_tree = new Node(yytext, "BREAK"); return BREAK;}
{IF}   {yylval.token_tree = new Node(yytext, "IF"); return IF;}
{ELSE}   {yylval.token_tree = new Node(yytext, "ELSE"); return ELSE;}
{RETURN}   {yylval.token_tree = new Node(yytext, "RETURN"); return RETURN;}


{INT}   {yylval.token_tree = new Node(yytext, "INT"); return INT;}
{FLOAT} {yylval.token_tree = new Node(yytext, "FLOAT"); return FLOAT;}  
{CHAR}  {yylval.token_tree = new Node(yytext, "CHAR"); return CHAR;} 
{STRING}    {yylval.token_tree = new Node(yytext, "STRING"); return STRING;}
{BOOL}  {yylval.token_tree = new Node(yytext, "BOOL"); return BOOL;}
{TYPE}   {yylval.token_tree = new Node(yytext, "TYPE"); return TYPE;}

{CR}  {;}
{LF} {yycolumn=1;}
{TAB}  {;}

{NAME}  {yylval.token_tree = new Node(yytext, "NAME"); return NAME;}

{COMMENTMULTILINE} {}
{COMMENTLINE} {}

.   {
        mistake++;
        printf("Error Occurs at Line %d: Unknown characters \'%s\'\n", yylineno, yytext);
    }

%%
int yywrap(void) {
    return 1;
}

