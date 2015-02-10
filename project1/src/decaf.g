
%token RETURN CALLOUT
%token BOOLTYPE INTTYPE CLASS VOID
%token IF ELSE FOR CONTINUE BREAK
 
%token IDENTIFIER INTEGER BOOLEAN CHARACTER STRING
%token EQUAL PLUSEQUAL MINUSEQUAL
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA SEMI BANG
%token MOD LOR LAND

%left PLUS MINUS CEQ CNE CLT CLE CGT CGE 
%left MUL DIV
%right UNARY

%start program

%%

program 
	: CLASS ident LBRACE field_decl_list method_decl_list RBRACE
	| CLASS ident LBRACE field_decl_list RBRACE
	| CLASS ident LBRACE method_decl_list RBRACE
	| CLASS ident LBRACE RBRACE
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
	: type ident LPAREN argument_list RPAREN block
	| type ident LPAREN RPAREN block
	;

block 
	: LBRACE var_decl_list statement_list RBRACE
	| LBRACE statement_list RBRACE
	| LBRACE var_decl_list RBRACE
	| LBRACE RBRACE
	;

var_decl_list 
	: var_decl
	| var_decl_list var_decl
	;
	
var_decl 
	: type ident_list SEMI
	;

statement_list 
	: statement
	| statement_list statement
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
	: location assign_op expr SEMI
	| method_call SEMI
	| IF LPAREN expr RPAREN block
	| IF LPAREN expr RPAREN block ELSE block
	| FOR ident EQUAL expr COMMA expr block
	| RETURN SEMI
	| RETURN expr SEMI
	| BREAK SEMI
	| CONTINUE SEMI
	| block
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
    
expr_list 
	: expr
	| expr_list COMMA expr
	;
	
expr 
	: location
	| method_call
	| literal
	| expr bin_op expr
	| LPAREN expr RPAREN
	| unary_op expr  %prec UNARY
	;

callout_arg_list 
	: callout_arg
	| callout_arg_list COMMA callout_arg
	;
	
callout_arg 
	: expr
	| STRING
	;
		
bin_op 
	: arith_op
	| rel_op
	| eq_op
	| cond_op
	;
	
unary_op
	: MINUS
	| PLUS
	| BANG
	;
	
arith_op 
	: PLUS
	| MINUS
	| MUL 
	| DIV
	| MOD
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
	
cond_op 
	: LAND 
	| LOR
	;
	
literal 
	: INTEGER 
	| BOOLEAN 
	| CHARACTER
	;
