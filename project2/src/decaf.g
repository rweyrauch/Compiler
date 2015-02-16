
%scanner Scanner.h
%scanner-token-function scanner.lex()

%union {
    Decaf::IrClass *programClass;
    Decaf::IrBlock *block;
    Decaf::IrMethodDecl *methodDecl;
    Decaf::IrFieldDecl *fieldDecl;
    Decaf::IrStatement *stmt;
    std::string *string;
}

%token RETURN CALLOUT
%token BOOLTYPE INTTYPE CLASS VOID
%token IF ELSE FOR CONTINUE BREAK
 
%token<string> IDENTIFIER INTEGER BOOLEAN CHARACTER STRING
%token EQUAL PLUSEQUAL MINUSEQUAL
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA SEMI BANG
%token MOD LOR LAND

%token PLUS MINUS CEQ CNE CLT CLE CGT CGE 
%token MUL DIV

%type <programClass> program
%type <block> block statement_list
%type <string> ident
%type <methodDecl> method_decl
%type <fieldDecl> field_decl
%type <stmt> statement
%start program

%%

program 
	: CLASS ident LBRACE field_decl_list method_decl_list RBRACE { $$ = new Decaf::IrClass(0, 0, *$2); $$->addFieldDecl($4); $$->addMethodDecl($5); }
	| CLASS ident LBRACE field_decl_list RBRACE  { $$ = new Decaf::IrClass(0, 0, *$2); $$->addFieldDecl($4);}
	| CLASS ident LBRACE method_decl_list RBRACE  { $$ = new Decaf::IrClass(0, 0, *$2); $$->addMethodDecl($4); }
	| CLASS ident LBRACE RBRACE  { $$ = new Decaf::IrClass(0, 0, *$2); }
	;
	
array_decl 
	: ident LBRACKET INTEGER RBRACKET
	;

field_var_decl_list
	: field_var_decl
	| field_var_decl_list COMMA field_var_decl
	;
	
field_var_decl
	: array_decl
	| ident
	;
	
field_decl_list 
	: field_decl
	| field_decl_list field_decl
	;
	
field_decl 
	: type field_var_decl_list SEMI
	;

method_decl_list 
	: method_decl
	| method_decl_list method_decl
	;
	
argument_list 
	: argument
	| argument_list COMMA argument
	;

argument 
	: type ident
	;
	
method_decl 
	: type ident LPAREN argument_list RPAREN block { $$ = new Decaf::IrMethodDecl(0, 0, *$2); }
	| type ident LPAREN RPAREN block { $$ = new Decaf::IrMethodDecl(0, 0, *$2); }
	;

block 
	: LBRACE var_decl_list statement_list RBRACE { $$ = $3; }
	| LBRACE statement_list RBRACE { $$ = $2; }
	| LBRACE var_decl_list RBRACE
	| LBRACE RBRACE { $$ = new Decaf::IrBlock(0,0); }
	;

var_decl_list 
	: var_decl
	| var_decl_list var_decl
	;
	
var_decl 
	: type ident_list SEMI
	;

statement_list 
	: statement { $$ = new Decaf::IrBlock(0, 0); $$->addStatement($<stmt>1); }
	| statement_list statement { $1->addStatement($<stmt>2); }
	;
	
type 
	: INTTYPE
	| BOOLTYPE
	| VOID
	;
	
ident_list 
	: ident
	| ident_list COMMA ident
	;
	
ident 
	: IDENTIFIER
	;
	  	
statement 
	: IF LPAREN expr RPAREN block { $$ = new Decaf::IrIfStatement(0, 0); }
	| IF LPAREN expr RPAREN block ELSE block  { $$ = new Decaf::IrIfStatement(0, 0); }
	| FOR ident EQUAL expr COMMA expr block { $$ = new Decaf::IrForStatement(0, 0); }
	| RETURN SEMI { $$ = new Decaf::IrReturnStatement(0, 0); }
	| RETURN expr SEMI { $$ = new Decaf::IrReturnStatement(0, 0); }
	| BREAK SEMI { $$ = new Decaf::IrBreakStatement(0, 0); }
	| CONTINUE SEMI { $$ = new Decaf::IrContinueStatement(0, 0); }
    | expr SEMI { $$ = new Decaf::IrExpressionStatement(0, 0); }
	| block { $$ = new Decaf::IrBlock(0, 0); }
	;
	
assign_op 
	: EQUAL
	| PLUSEQUAL
	| MINUSEQUAL
	;
	
method_call 
	: method_name LPAREN expr_list RPAREN
	| method_name LPAREN RPAREN
	| CALLOUT LPAREN STRING COMMA callout_arg_list RPAREN
	| CALLOUT LPAREN STRING RPAREN
	;
	
method_name 
	: ident
	;
	
location 
	: ident
	| ident LBRACKET expr RBRACKET
	;
	
primary_expr
    : location
    | literal
    | method_call
	| LPAREN expr RPAREN
    ;
    
unary_expr
    : primary_expr
    | unary_op unary_expr
    ;
  
mult_expr
    : unary_expr
    | mult_expr MUL unary_expr
    | mult_expr DIV unary_expr
    | mult_expr MOD unary_expr
    ;
    
add_expr
    : mult_expr
    | add_expr PLUS mult_expr
    | add_expr MINUS mult_expr
    ;

rel_expr
    : add_expr
    | rel_expr rel_op add_expr
    ;
    
equal_expr
    : rel_expr
    | equal_expr eq_op
    ;

logic_expr
    : equal_expr
    | logic_expr logic_op equal_expr
    ;
    
assign_expr
    : logic_expr
    | assign_expr assign_op logic_expr
    ;
    
expr 
	: assign_expr
	;

expr_list 
	: expr
	| expr_list COMMA expr
	;

callout_arg_list 
	: callout_arg
	| callout_arg_list COMMA callout_arg
	;
	
callout_arg 
	: expr
	| STRING
	;
	
unary_op
	: MINUS
	| BANG
	;

rel_op 
	: CLT
	| CGT
	| CLE
	| CGE
	;
	
eq_op 
	: CEQ 
	| CNE
	;
	
logic_op 
	: LAND 
	| LOR
	;
	
literal 
	: INTEGER 
	| BOOLEAN 
	| CHARACTER
	;
