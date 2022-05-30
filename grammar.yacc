
%{
    #include <stdio.h>
    #include "ast.h"
    #include "type.h"
    int yylex(void);
    int mistakeRecord[4096]={0};
    int mistake = 0;
    extern Node *ROOT;
    void yyerror (char const *s) {
        fprintf (stderr, "%s\n", s);
    }
%}

%nonassoc LELSE
%nonassoc ELSE


%union
{
    struct Node* token_tree;
}


%token <token_tree> TYPE

%token <token_tree> LP RP LMB RMB LC RC
%token <token_tree> LETH MOTH LETHE MOTHE ISE NOTE

%token <token_tree> PLUS MINUS STAR DIVISION

%token <token_tree> COMMA
%token <token_tree> SEMICOLON
%token <token_tree> ASSIGN

%token <token_tree> AND OR NOT

%token <token_tree> WHILE
%token <token_tree> BREAK
%token <token_tree> IF
%token <token_tree> ELSE
%token <token_tree> RETURN

%token <token_tree> INT
%token <token_tree> FLOAT
%token <token_tree> CHAR
%token <token_tree> BOOL
%token <token_tree> STRING
%token <token_tree> NAME


%right ASSIGN
%left OR AND
%left LETH MOTH LETHE MOTHE ISE NOTE
%left PLUS MINUS
%left STAR DIVISION
%right NOT
%left LP RP LMB RMB

%type <token_tree> Allprogram
%type <token_tree> Datatype
%type <token_tree> Define
%type <token_tree> Definelist
%type <token_tree> Defineblock
%type <token_tree> Variable
%type <token_tree> Function
%type <token_tree> Variableline
%type <token_tree> Singledef
%type <token_tree> Op
%type <token_tree> Opblock
%type <token_tree> Declaration
%type <token_tree> Declarationline
%type <token_tree> Scope
%type <token_tree> Def
%type <token_tree> Defineformallist
%type <token_tree> Variablelist


%%
Allprogram: 
	Definelist {
		$$ = new Node("", "Allprogram", 1, $1);
		ROOT = $$;
	};

Datatype: 
	TYPE {
        $$ = new Node("", "Datatype", 1, $1);
	}
    ;


Define: 
	Datatype Defineblock SEMICOLON {
        $$ = new Node("", "Define", 3, $1, $2, $3);
	}
    | Datatype Function Scope {
        $$ = new Node("", "Define", 3, $1, $2, $3);
	}
    ;

Definelist: 
	Define Definelist {
		$$ = new Node("", "Definelist", 2, $1, $2);
	}
    | %empty {
		$$ = nullptr;
	}    
    ;

Defineblock: 
	Variable {
		$$ = new Node("", "Defineblock", 1, $1);
	}
    | Variable COMMA Defineblock {
		$$ = new Node("", "Defineblock", 3, $1, $2, $3);
	}
    ;



Variable:  
	NAME {
        $$ = new Node("", "Variable", 1, $1);
	}
    // only support one-dimensional array
    | NAME LMB INT RMB {
        $$ = new Node("", "Variable", 4, $1, $2, $3, $4);
	}
    // used in function definition
    | NAME LMB RMB {
        $$ = new Node("", "Variable", 3, $1, $2, $3);
	}
    | NAME LMB error RMB {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
        	printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    ;

Function:  
	NAME LP Variableline RP {
        $$ = new Node("", "Function", 4, $1, $2, $3, $4);
	}
    | NAME LP RP {
        $$ = new Node("", "Function", 3, $1, $2, $3);
	}
    | NAME LP error RP {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | NAME error RP {
        if(mistakeRecord[@2.first_line-1] == 0){
            mistakeRecord[@2.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.first_line);
        }
    }
    ;

Variableline: 
	Singledef COMMA Variableline {
        $$ = new Node("", "Variableline", 3, $1, $2, $3);
	}
    | Singledef {
        $$ = new Node("", "Variableline", 1, $1);
	}
    ;

Singledef: 
	Datatype Variable {
        $$ = new Node("", "Singledef", 2, $1, $2);
	}
    ;


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
    | LP Op RP {
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
    | NAME LP Opblock RP {
        $$ = new Node("", "Op", 4, $1, $2, $3, $4);
        $$->setValueType($1->getValueType());
    }
    | NAME LP RP {
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
    | FLOAT {
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
    }
    | Op ASSIGN error SEMICOLON {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op AND error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op OR error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op PLUS error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op MINUS error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op STAR error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op DIVISION error SEMICOLON  {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | NAME LP error SEMICOLON {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.first_line);
        }
    }
    | Op LMB error RMB {
        if(mistakeRecord[@3.first_line-1] == 0){
            mistakeRecord[@3.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: wrong Expression between \"[\" and \"]\".\n", @3.first_line);
        }
    }
    ;


Opblock:  
    Op COMMA Opblock {
        $$ = new Node("", "Opblock", 3, $1, $2, $3);
    }
    | Op {
        $$ = new Node("", "Opblock", 1, $1);
    }
    ;



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
    | IF LP Op RP Declaration %prec LELSE {
		$$ = new Node("", "Declaration", 5, $1, $2, $3, $4, $5);
	}
    | IF LP Op RP Declaration ELSE Declaration {
		$$ = new Node("", "Declaration", 7, $1, $2, $3, $4, $5, $6, $7);
	}
    | IF error ELSE Declaration {
        if(mistakeRecord[@2.first_line-1] == 0){
            mistakeRecord[@2.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.first_line);
        }
    }
    | WHILE LP Op RP Declaration {
		$$ = new Node("", "Declaration", 5, $1, $2, $3, $4, $5);
	}
    | WHILE error RP {
        if(mistakeRecord[@2.first_line-1] == 0){
            mistakeRecord[@2.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.first_line);
        }
    }
    | WHILE error RC {
        if(mistakeRecord[@2.first_line-1] == 0){
            mistakeRecord[@2.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.first_line);
        }
    }
    | error SEMICOLON {
    	if(mistakeRecord[@1.first_line-1] == 0){
    	    mistakeRecord[@1.first_line-1] = 1;
    	    mistake++;
    	    printf("Error Occur at Line %d: Syntax Error.\n", @1.first_line);
    	}
    }
    | Op {
        if(mistakeRecord[@1.last_line-1] == 0){
            mistakeRecord[@1.last_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @1.last_line);
        }
    }
    | RETURN Op {
        if(mistakeRecord[@2.last_line-1] == 0){
            mistakeRecord[@2.last_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.last_line);
        }
    }
    ;

Declarationline:  
	%empty {
		$$ = nullptr;
	}
    | Declaration Declarationline {
		$$ = new Node("", "Scope", 2, $1, $2);
	}
    ;

Scope:  
	LC Defineformallist Declarationline RC {	
		$$ = new Node("", "Scope", 4, $1, $2, $3, $4);
	}
    | LC Defineformallist Declarationline{
        if(mistakeRecord[@3.last_line-1] == 0){
            mistakeRecord[@3.last_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @3.last_line);
        }
    }
    | error RC{
        if(mistakeRecord[@1.first_line-1] == 0){
            mistakeRecord[@1.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @1.first_line);
        }
    }
    ;

Def:  
	Datatype Variablelist SEMICOLON {
		$$ = new Node("", "Def", 3, $1, $2, $3);
    }
    | Datatype error SEMICOLON {
        if(mistakeRecord[@2.first_line-1] == 0){
            mistakeRecord[@2.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.first_line);
        }
    }
    | error Variablelist SEMICOLON {
        if(mistakeRecord[@1.first_line-1] == 0){
            mistakeRecord[@1.first_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @1.first_line);
        }
    }
    | Datatype Variablelist {
        if(mistakeRecord[@2.last_line-1] == 0){
            mistakeRecord[@2.last_line-1] = 1;
            mistake++;
            printf("Error Occur at Line %d: Syntax Error.\n", @2.last_line);
        }
    }
    ;



Defineformallist:  
	%empty {
		$$ = nullptr;
	}
    | Def Defineformallist {
		$$ = new Node("", "Defineformallist", 2, $1, $2);
	}
    ;

Variablelist:  
	Variable {
		$$ = new Node("", "Variablelist", 1, $1);
	}
    | Variable COMMA Variablelist {
		$$ = new Node("", "Variablelist", 3, $1, $2, $3);
	}
    ;




%%
