
%scanner Scanner.h
//%scanner-token-function scanner.lex()

%union {
    Decaf::IrClass *programClass;
    Decaf::IrIdentifier *ident;
    std::vector<Decaf::IrIdentifier*> *identList;
    Decaf::IrLocation *location;
    std::vector<Decaf::IrLocation*> *locationList;
    Decaf::IrBlock *block;
    Decaf::IrMethodCall *methodCall;
    Decaf::IrMethodDecl *methodDecl;
    Decaf::IrFieldDecl *fieldDecl;
    Decaf::IrExpression *expression;
    std::vector<Decaf::IrMethodDecl*> *methodDeclList;
    std::vector<Decaf::IrFieldDecl*> *fieldDeclList;
    std::vector<std::vector<Decaf::IrFieldDecl*>> *fieldDeclListList;
    Decaf::IrStatement *stmt;
    std::vector<Decaf::IrStatement*> *stmtList;
    Decaf::IrVariableDecl *varDecl;
    std::vector<Decaf::IrVariableDecl*> *varDeclList;
    Decaf::IrLiteral *literal;
    std::string *string;
    int token;
}

%token RETURN CALLOUT
%token BOOLTYPE INTTYPE CLASS VOID
%token IF ELSE FOR CONTINUE BREAK

%token IDENTIFIER INTEGER BOOLEAN CHARACTER STRING
%token EQUAL PLUSEQUAL MINUSEQUAL
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA SEMI BANG
%token MOD LOR LAND

%token PLUS MINUS CEQ CNE CLT CLE CGT CGE 
%token MUL DIV

%type <programClass> program
%type <block> block
%type <methodDecl> method_decl
%type <methodDeclList> method_decl_list
%type <fieldDeclList> field_decl
%type <fieldDeclListList> field_decl_list
%type <methodCall> method_call
%type <stmt> statement
%type <stmtList> statement_list
%type <varDecl> var_decl argument_decl
%type <varDeclList> var_decl_list argument_decl_list
%type <ident> ident
%type <identList> ident_list
%type <token> type
%type <literal> literal
%type <location> location
%type <locationList> location_list
%type <expression> expr primary_expr unary_expr mult_expr add_expr rel_expr logic_expr assign_expr

%start program

%%

program 
    : CLASS ident LBRACE field_decl_list method_decl_list RBRACE 
    { 
        $$ = new Decaf::IrClass(d_scanner.lineNr(), d_scanner.columnNr(), $2); 
        setRoot($$); 
        for (auto it : *$4)
        {
            $$->addFieldDecl(it); 
        }
        $$->addMethodDecl(*$5); 
    }
    | CLASS ident LBRACE field_decl_list RBRACE  
    { 
        $$ = new Decaf::IrClass(d_scanner.lineNr(), d_scanner.columnNr(), $2); 
        setRoot($$); 
        for (auto it : *$4)
        {
            $$->addFieldDecl(it); 
        }
    }
    | CLASS ident LBRACE method_decl_list RBRACE  
    { 
        $$ = new Decaf::IrClass(d_scanner.lineNr(), d_scanner.columnNr(), $2); 
        setRoot($$); 
        $$->addMethodDecl(*$4); 
    }
    | CLASS ident LBRACE RBRACE  
    { 
        $$ = new Decaf::IrClass(d_scanner.lineNr(), d_scanner.columnNr(), $2); 
        setRoot($$); 
    }
    ;
    
location_list
    : location 
    { 
        $$ = new std::vector<Decaf::IrLocation*>(); 
        $$->push_back($1); 
    }
    | location_list COMMA location 
    { 
        $1->push_back($3); 
    }
    ;
        
field_decl_list 
    : field_decl 
    { 
        $$ = new std::vector<std::vector<Decaf::IrFieldDecl*>>(); 
        $$->push_back(*$<fieldDeclList>1); 
    }
    | field_decl_list field_decl 
    { 
        $1->push_back(*$<fieldDeclList>2); 
    }
    ;
    
field_decl 
    : type location_list SEMI 
    { 
        $$ = new std::vector<Decaf::IrFieldDecl*>(); 
        for (auto it = $2->begin(); it != $2->end(); ++it) 
        { 
            Decaf::IrLocation* location = *it; 
            Decaf::IrFieldDecl* decl = new Decaf::IrFieldDecl(d_scanner.lineNr(), d_scanner.columnNr(), location->getIdentifier(), (Decaf::IrType)$1); 
            $$->push_back(decl); 
        } 
    }
    ;

method_decl_list 
    : method_decl 
    { 
        $$ = new std::vector<Decaf::IrMethodDecl*>(); 
        $$->push_back($<methodDecl>1); 
    }
    | method_decl_list method_decl 
    { 
        $1->push_back($<methodDecl>2); 
    }
    ;
    
argument_decl_list 
    : argument_decl 
    { 
        $$ = new std::vector<Decaf::IrVariableDecl*>(); 
        $$->push_back($<varDecl>1); 
    }
    | argument_decl_list COMMA argument_decl 
    { 
        $1->push_back($<varDecl>3); 
    }
    ;

argument_decl 
    : type ident 
    { 
        $$ = new Decaf::IrVariableDecl(d_scanner.lineNr(), d_scanner.columnNr(), $2, (Decaf::IrType)$1); 
    }
    ;
    
method_decl 
    : type ident LPAREN argument_decl_list RPAREN block 
    { 
        $$ = new Decaf::IrMethodDecl(d_scanner.lineNr(), d_scanner.columnNr(), $2 ); 
    }
    | type ident LPAREN RPAREN block 
    { 
        $$ = new Decaf::IrMethodDecl(d_scanner.lineNr(), d_scanner.columnNr(), $2);
    }
    ;

block 
    : LBRACE var_decl_list statement_list RBRACE 
    { 
        $$ = new Decaf::IrBlock(d_scanner.lineNr(), d_scanner.columnNr()); 
        $$->addVariableDecl(*$2); 
        $$->addStatements(*$3); 
    }
    | LBRACE statement_list RBRACE 
    {
        $$ = new Decaf::IrBlock(d_scanner.lineNr(), d_scanner.columnNr()); 
        $$->addStatements(*$2); 
    }
    | LBRACE var_decl_list RBRACE 
    { 
        $$ = new Decaf::IrBlock(d_scanner.lineNr(), d_scanner.columnNr()); 
        $$->addVariableDecl(*$2); 
    }
    | LBRACE RBRACE 
    { 
        $$ = new Decaf::IrBlock(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    ;

var_decl_list 
    : var_decl 
    { 
        $$ = new std::vector<Decaf::IrVariableDecl*>(); 
        $$->push_back($<varDecl>1); 
    }
    | var_decl_list var_decl 
    { 
        $1->push_back($<varDecl>2); 
    }
    ;
    
var_decl 
    : type ident_list SEMI 
    { 
        $$ = new Decaf::IrVariableDecl(d_scanner.lineNr(), d_scanner.columnNr(), *$2, (Decaf::IrType)$1); 
    }
    ;

statement_list 
    : statement 
    { 
        $$ = new std::vector<Decaf::IrStatement*>(); 
        $$->push_back($1); 
    }
    | statement_list statement 
    { 
        $1->push_back($2); 
    }
    ;
    
type 
    : INTTYPE 
    { 
        $$ = (int)Decaf::IrType::Integer; 
    }
    | BOOLTYPE 
    { 
        $$ = (int)Decaf::IrType::Boolean; 
    }
    | VOID 
    { 
        $$ = (int)Decaf::IrType::Void; 
    }
    ;
    
ident_list 
    : ident 
    { 
        $$ = new std::vector<IrIdentifier*>(); 
        $$->push_back($1); 
    }
    | ident_list COMMA ident 
    {
        $1->push_back($3); 
    }
    ;
    
ident 
    : IDENTIFIER 
    { 
        $$ = new Decaf::IrIdentifier(d_scanner.lineNr(), d_scanner.columnNr(), d_scanner.matched()); 
    }
    ;

statement 
    : IF LPAREN expr RPAREN block 
    { 
        $$ = new Decaf::IrIfStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | IF LPAREN expr RPAREN block ELSE block  
    { 
        $$ = new Decaf::IrIfStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | FOR ident EQUAL expr COMMA expr block 
    { 
        $$ = new Decaf::IrForStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | RETURN SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | RETURN expr SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | BREAK SEMI 
    { 
        $$ = new Decaf::IrBreakStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | CONTINUE SEMI 
    {   
        $$ = new Decaf::IrContinueStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | expr SEMI 
    { 
        $$ = new Decaf::IrExpressionStatement(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    | block 
    { 
        $$ = new Decaf::IrBlock(d_scanner.lineNr(), d_scanner.columnNr()); 
    }
    ;
    
assign_op 
    : EQUAL
    | PLUSEQUAL
    | MINUSEQUAL
    ;
    
method_call 
    : ident LPAREN expr_list RPAREN 
    { 
        $$ = new Decaf::IrMethodCall(d_scanner.lineNr(), d_scanner.columnNr(), $1, Decaf::IrType::Unknown); 
    }
    | ident LPAREN RPAREN 
    { 
        $$ = new Decaf::IrMethodCall(d_scanner.lineNr(), d_scanner.columnNr(), $1, Decaf::IrType::Unknown); 
    }
    | CALLOUT LPAREN STRING COMMA callout_arg_list RPAREN
    | CALLOUT LPAREN STRING RPAREN
    ;
    
location 
    : ident 
    { 
        $$ = new Decaf::IrLocation(d_scanner.lineNr(), d_scanner.columnNr(), $1, Decaf::IrType::Unknown); 
    }
    | ident LBRACKET expr RBRACKET 
    { 
        $$ = new Decaf::IrLocation(d_scanner.lineNr(), d_scanner.columnNr(), $1, Decaf::IrType::Unknown, $3); 
    }
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
 
logic_expr
    : rel_expr
    | logic_expr logic_op rel_expr
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
    | CEQ 
    | CNE
    ;
    
logic_op 
    : LAND 
    | LOR
    ;
    
literal 
    : INTEGER 
    { 
        $$ = new Decaf::IrIntegerLiteral(d_scanner.lineNr(), d_scanner.columnNr(), d_scanner.matched()); 
    }
    | BOOLEAN 
    { 
        $$ = new Decaf::IrBooleanLiteral(d_scanner.lineNr(), d_scanner.columnNr(), d_scanner.matched()); 
    }
    | CHARACTER 
    { 
        $$ = new Decaf::IrCharacterLiteral(d_scanner.lineNr(), d_scanner.columnNr(), d_scanner.matched()); 
    }
    ;
