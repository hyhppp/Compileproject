%option yylineno
%{
int yylineno;
%}


alphabet [a-zA-Z]
number  [0-9]
numbers [0-9]+


%%

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
