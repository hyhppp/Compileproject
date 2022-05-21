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
COMMA ,
SEMICOLON ;
PLUS +
MINUS -


%%

{LB}   {return LB;}
{RB}   {return RB;}
{LMB}   {return LMB;}
{RMB}   {return RMB;}
{LETH}   {return LETH;}
{MOTH}   {return MOTH;}
{COMMA}   {return COMMA;}
{SEMICOLON}   {return SEMICOLON;}
{PLUS}   {return PLUS;}
{MINUS}   {return MINUS;}



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
