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

FOR for
WHILE while
BREAK break
IF if
ELSE else
RETURN return


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

{FOR}   {return FOR;}
{WHILE}   {return WHILE;}
{BREAK}   {return BREAK;}
{IF}   {return IF;}
{ELSE}   {return ELSE;}
{RETURN}   {return RETURN;}


%%
