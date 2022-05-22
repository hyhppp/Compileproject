%{
    #include <stdio.h>
    #include "ast.h"
    #include "type.h"
    int yylex(void);
    extern Node *ROOT;
    int mistake = 0;
%}

%nonassoc LELSE
%nonassoc ELSE

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

Op:  
	Op ASSIGN Op {
		$$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($3->getValueType());
	}
    	| Op AND Op {
		$$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
	}
    	| Op OR Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op LETH Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op MOTH Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op LETHE Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op MOTHE Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op ISE Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op NOTE Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op PLUS Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op MINUS Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op STAR Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| Op DIVISION Op {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| LB Op RB {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($2->getValueType());
    	}
    	| MINUS Op {
        $$ = new Node("", "Op", 2, $1, $2);
        $$->setValueType($2->getValueType());
    	}
    	| NOT Op {
        $$ = new Node("", "Op", 2, $1, $2);
        $$->setValueType(TYPE_BOOL);
    	}
    	| NAME LB Opblock RB {
        $$ = new Node("", "Op", 4, $1, $2, $3, $4);
        $$->setValueType($1->getValueType());
    	}
    	| NAME LB RB {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType());
    	}
    	| NAME LMB Op RMB {
        $$ = new Node("", "Op", 4, $1, $2, $3, $4);
        $$->setValueType($1->getValueType() - ARRAY);
    	}
    	| NAME LMB RMB {
        $$ = new Node("", "Op", 3, $1, $2, $3);
        $$->setValueType($1->getValueType() + ARRAY);
    	}
    	| NAME {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType($1->getValueType());
    	}
    	| INT {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType(TYPE_INT);
    	}
    	| DOUBLE {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType(TYPE_FLOAT);
    	}
    	| BOOL {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType(TYPE_BOOL);
    	}
    	| CHAR {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType(TYPE_CHAR);
    	}
    	| STRING {
        $$ = new Node("", "Op", 1, $1);
        $$->setValueType(TYPE_CHAR_ARRAY);
    	};



Opblock:  
    	Opblock COMMA Opblock {
        $$ = new Node("", "Opblock", 3, $1, $2, $3);
    	}
    	| Op {
        $$ = new Node("", "Opblock", 1, $1);
    	};
	
Declaration:
	Op SEMICOLON {
	$$ = new Node("", "Declaration", 2, $1, $2);
	}
    	| Scope {
	$$ = new Node("", "Declaration", 1, $1);
	}
    	| RETURN Op SEMICOLON {
	$$ = new Node("", "Declaration", 3, $1, $2, $3);
	}
    	| RETURN SEMICOLON {
        $$ = new Node("", "Declaration", 2, $1, $2);
    	}
    	| BREAK SEMICOLON {
        $$ = new Node("", "Declaration", 2, $1, $2);
    	}
    	| IF LB Op RB Declaration %prec LELSE {
	$$ = new Node("", "Declaration", 5, $1, $2, $3, $4, $5);
	}
    	| IF LB Op RB Declaration ELSE Declaration {
	$$ = new Node("", "Declaration", 7, $1, $2, $3, $4, $5, $6, $7);
	}
	
	
Declarationline:
	%empty {
	$$ = nullptr;
	}
    	| Declaration Declarationline {
	$$ = new Node("", "Declarationline", 2, $1, $2);
	};
	
Scope:  
	LBB Defineformallist Declarationline RBB {	
	$$ = new Node("", "Scope", 4, $1, $2, $3, $4);
	}

Defineformal:  
	Datatype Variablelist SEMICOLON {
	$$ = new Node("", "Defineformal", 3, $1, $2, $3);
    	};

Defineformallist:  
	%empty {
	$$ = nullptr;
	}
    	| Op Defineformallist {
	$$ = new Node("", "Defineformallist", 2, $1, $2);
	};

Variablelist:  
	Variable {
	$$ = new Node("", "Variablelist", 1, $1);
	}
    	| Variable COMMA Variablelist {
	$$ = new Node("", "Variablelist", 3, $1, $2, $3);
	};




%%
