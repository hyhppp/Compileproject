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



%token <token_tree> TYPE

%token <token_tree> LB RB LMB RMB LBB RBB
%token <token_tree> LETH MOTH LETHE MOTHE ISE NOTE

%token <token_tree> PLUS MINUS STAR DIVISION

%token <token_tree> COMMA
%token <token_tree> SEMICOLON
%token <token_tree> ASSIGN

%token <token_tree> AND OR NOT

%token <token_tree> FOR
%token <token_tree> WHILE
%token <token_tree> BREAK
%token <token_tree> IF
%token <token_tree> ELSE
%token <token_tree> RETURN

%token <token_tree> INT
%token <token_tree> DOUBLE
%token <token_tree> CHAR
%token <token_tree> BOOL
%token <token_tree> STRING

%right ASSIGNOP
%left OR AND
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LB RB LMB RMB


%type <token_tree> Allprogram
%type <token_tree> Datatype
%type <token_tree> Define
%type <token_tree> Definelist
%type <token_tree> Defineblock
%type <token_tree> Variable
%type <token_tree> Function
%type <token_tree> Variableline
%type <token_tree> Singledef
%type <token_tree> 
%type <token_tree> 
%type <token_tree> 




%%
Allprogram:
	Definelist {
		$$ = new Node("", "Allprogram", 1, $1);
		ROOT = $$;
	};
Datatype:
	TYPE {
        $$ = new Node("", "Datatype", 1, $1);
	};
Define:
	Datatype Definelist SEMICOLON {
        $$ = new Node("", "Define", 3, $1, $2, $3);
	}
    	| Datatype Function Scope {
        $$ = new Node("", "Define", 3, $1, $2, $3);
	};
Definelist: 
	Variable {
		$$ = new Node("", "Definelist", 1, $1);
	}
    	| Variable COMMA Definelist {
		$$ = new Node("", "Definelist", 3, $1, $2, $3);
	};
Defineblock:
	Define Defineblock {
		$$ = new Node("", "Defineblock", 2, $1, $2);
	}
    	| %empty {
		$$ = nullptr;
	}；


Variable:  
	NAME {
        $$ = new Node("", "Variable", 1, $1);
	}
    	| NAME LMB INT RMB {
        $$ = new Node("", "Variable", 4, $1, $2, $3, $4);
	}
    	| NAME LMB RMB {
        $$ = new Node("", "Variable", 3, $1, $2, $3);
	};

Function:
	NAME LB Variableline RB {
        $$ = new Node("", "Function", 4, $1, $2, $3, $4);
	}
    	| NAME LB RB {
        $$ = new Node("", "Function", 3, $1, $2, $3);
	};
	
Variableline:
	Singledef COMMA Variableline {
        $$ = new Node("", "Variableline", 3, $1, $2, $3);
	}
    	| Variableline {
        $$ = new Node("", "Variableline", 1, $1);
	};
	
Singledef:
	Datatype Variable {
        $$ = new Node("", "Singledef", 2, $1, $2);
	};
	





%%
