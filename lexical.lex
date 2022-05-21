%option yylineno
%{
int yylineno;
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

{LB}   {return LB;}
{RB}   {return RB;}
{LMB}   {return LMB;}
{RMB}   {return RMB;}
{LBB}   {return LBB;}
{RBB}   {return RBB;}
{LETH}   {return LETH;}
{MOTH}   {return MOTH;}
{LETHE}   {return LETHE;}
{MOTHE}   {return MOTHE;}
{ISE}   {return ISE;}
{NOTE}   {return NOTE;}


{PLUS}   {return PLUS;}
{MINUS}   {return MINUS;}
{STAR}   {return STAR;}
{DIVISION}   {return DIVISION;}

{COMMA}   {return COMMA;}
{SEMICOLON}   {return SEMICOLON;}
{ASSIGN}   {return ASSIGN;}
{AND}   {return AND;}
{OR}   {return OR;}
{NOT}   {return NOT;}

{FOR}   {return FOR;}
{WHILE}   {return WHILE;}
{BREAK}   {return BREAK;}
{IF}   {return IF;}
{ELSE}   {return ELSE;}
{RETURN}   {return RETURN;}

{INT}   {return INT;}
{DOUBLE}   {return DOUBLE;}
{CHAR}   {return CHAR;}
{STRING}   {return STRING;}
{BOOL}   {return BOOL;}
{TYPE}   {return TYPE;}

{CR}   {return CR;}
{LF}   {return LF;}
{TAB}   {return TAB;}

{COMMENT}   {}


%%

int yywrap(void) {
    return 1;
}
