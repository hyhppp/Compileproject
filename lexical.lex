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
LETH <
MOTH >
LETHE <=
MOTHE >=
ISE ==
NOTE !=
COMMA ,
SEMICOLON ;
PLUS +
MINUS -



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
{LETH}   {return LETH;}
{MOTH}   {return MOTH;}
{LETHE}   {return LETHE;}
{MOTHE}   {return MOTHE;}
{ISE}   {return ISE;}
{NOTE}   {return NOTE;}
{COMMA}   {return COMMA;}
{SEMICOLON}   {return SEMICOLON;}
{PLUS}   {return PLUS;}
{MINUS}   {return MINUS;}

{FOR}   {return FOR;}
{WHILE}   {return WHILE;}
{BREAK}   {return BREAK;}
{IF}   {return IF;}
{ELSE}   {return ELSE;}
{RETURN}   {return RETURN;}



({alphabet}|_)({alphabet}|{number}|_)* {
	char * temp = strdup("%");
	strcat(temp, yytext);
	yylval.str=temp; 
	return ID;}
{number}+	{
  yylval.str=strdup(yytext);
  return NUM;}
{number}+\.{number}+ 	{
  yylval.str=strdup(yytext); 
  return REAL;}

%%
