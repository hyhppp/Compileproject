%{
    #include <stdio.h>
    #include "ast.h"
    #include "type.h"
    int yylex(void);
    int mistake = 0;
%}

%union
{
    struct Node* token_tree;
}



%token <label_tree> TYPE

%token <label_tree> LB RB LMB RMB LBB RBB
%token <label_tree> LETH MOTH LETHE MOTHE ISE NOTE

%token <label_tree> PLUS MINUS STAR DIVISION

%token <label_tree> COMMA
%token <label_tree> SEMICOLON
%token <label_tree> ASSIGN

%token <label_tree> AND OR NOT

%token <label_tree> FOR
%token <label_tree> WHILE
%token <label_tree> BREAK
%token <label_tree> IF
%token <label_tree> ELSE
%token <label_tree> RETURN

%token <label_tree> INT
%token <label_tree> DOUBLE
%token <label_tree> CHAR
%token <label_tree> BOOL
%token <label_tree> STRING





%%


%%
