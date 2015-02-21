
%scanner Scanner.h
%lsp-needed 

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
    std::vector<Decaf::IrExpression*> *exprList;
    std::vector<Decaf::IrMethodDecl*> *methodDeclList;
    std::vector<Decaf::IrFieldDecl*> *fieldDeclList;
    std::vector<std::vector<Decaf::IrFieldDecl*>> *fieldDeclListList;
    Decaf::IrStatement *stmt;
    std::vector<Decaf::IrStatement*> *stmtList;
    Decaf::IrVariableDecl *varDecl;
    std::vector<Decaf::IrVariableDecl*> *varDeclList;
    Decaf::IrLiteral *literal;
    Decaf::IrStringLiteral *stringLiteral;
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
%type <token> type rel_op logic_op assign_op
%type <literal> literal
%type <stringLiteral> string_literal
%type <location> location
%type <locationList> location_list
%type <exprList> expr_list
%type <expression> expr primary_expr unary_expr mult_expr add_expr rel_expr logic_expr assign_expr

%start program

%%

program 
    : CLASS ident LBRACE field_decl_list method_decl_list RBRACE 
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, $2); 
        setRoot($$); 
        for (auto it : *$4)
        {
            $$->addFieldDecl(it); 
        }
        $$->addMethodDecl(*$5); 
    }
    | CLASS ident LBRACE field_decl_list RBRACE  
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, $2); 
        setRoot($$); 
        for (auto it : *$4)
        {
            $$->addFieldDecl(it); 
        }
    }
    | CLASS ident LBRACE method_decl_list RBRACE  
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, $2); 
        setRoot($$); 
        $$->addMethodDecl(*$4); 
    }
    | CLASS ident LBRACE RBRACE  
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, $2); 
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
            Decaf::IrFieldDecl* decl = new Decaf::IrFieldDecl(@1.first_line, @1.first_column, location, (Decaf::IrType)$1); 
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
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, $2, (Decaf::IrType)$1); 
    }
    ;
    
method_decl 
    : type ident LPAREN argument_decl_list RPAREN block 
    { 
        $$ = new Decaf::IrMethodDecl(@1.first_line, @1.first_column, $2, (Decaf::IrType)$1); 
        for (auto it : *$4)
        {
            $$->addArgument(it);
        }
        $$->addBlock($6);
    }
    | type ident LPAREN RPAREN block 
    { 
        $$ = new Decaf::IrMethodDecl(@1.first_line, @1.first_column, $2, (Decaf::IrType)$1);
        $$->addBlock($5);
    }
    ;

block 
    : LBRACE var_decl_list statement_list RBRACE 
    { 
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column); 
        $$->addVariableDecl(*$2); 
        $$->addStatements(*$3); 
    }
    | LBRACE statement_list RBRACE 
    {
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column); 
        $$->addStatements(*$2); 
    }
    | LBRACE var_decl_list RBRACE 
    { 
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column); 
        $$->addVariableDecl(*$2); 
    }
    | LBRACE RBRACE 
    { 
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column); 
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
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, *$2, (Decaf::IrType)$1); 
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
        $$ = new Decaf::IrIdentifier(@1.first_line, @1.first_column, d_scanner.matched()); 
    }
    ;

statement 
    : IF LPAREN expr RPAREN block 
    { 
        $$ = new Decaf::IrIfStatement(@1.first_line, @1.first_column, $3, $5); 
    }
    | IF LPAREN expr RPAREN block ELSE block  
    { 
        $$ = new Decaf::IrIfStatement(@1.first_line, @1.first_column, $3, $5, $7); 
    }
    | FOR ident EQUAL expr COMMA expr block 
    { 
        $$ = new Decaf::IrForStatement(@1.first_line, @1.first_column, $2, $4, $6, $7); 
    }
    | RETURN SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(@1.first_line, @1.first_column); 
    }
    | RETURN expr SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(@1.first_line, @1.first_column, $2); 
    }
    | BREAK SEMI 
    { 
        $$ = new Decaf::IrBreakStatement(@1.first_line, @1.first_column); 
    }
    | CONTINUE SEMI 
    {   
        $$ = new Decaf::IrContinueStatement(@1.first_line, @1.first_column); 
    }
    | expr SEMI 
    { 
        $$ = new Decaf::IrExpressionStatement(@1.first_line, @1.first_column, $1); 
    }
    | block 
    { 
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column); 
    }
    ;
    
assign_op 
    : EQUAL { $$ = (int)Decaf::IrAssignmentOperator::Assign; }
    | PLUSEQUAL { $$ = (int)Decaf::IrAssignmentOperator::IncrementAssign; }
    | MINUSEQUAL { $$ = (int)Decaf::IrAssignmentOperator::DecrementAssign; }
    ;
    
method_call 
    : ident LPAREN expr_list RPAREN 
    { 
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, $1, Decaf::IrType::Unknown);
        for (auto it : *$3) 
        {
            $$->addArgument(it);
        }
    }
    | ident LPAREN RPAREN 
    { 
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, $1, Decaf::IrType::Unknown); 
    }
    | CALLOUT LPAREN string_literal COMMA expr_list RPAREN
    {
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, $3, Decaf::IrType::Unknown);
        for (auto it : *$5) 
        {
            $$->addArgument(it);
        }
    }
    | CALLOUT LPAREN string_literal RPAREN
    {
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, $3, Decaf::IrType::Unknown);
    }
    ;
    
location 
    : ident 
    { 
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, $1, Decaf::IrType::Unknown); 
    }
    | ident LBRACKET expr RBRACKET 
    { 
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, $1, Decaf::IrType::Unknown, $3); 
    }
    ;

primary_expr
    : location { $$ = $1; }
    | literal { $$ = $1; }
    | string_literal { $$ = $1; }
    | method_call { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    ;
    
unary_expr
    : primary_expr { $$ = $1; }
    | MINUS unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, nullptr, Decaf::IrBinaryOperator::Subtract, $2); 
    }
    | BANG unary_expr  
    { 
        $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, nullptr, Decaf::IrBooleanOperator::Not, $2); 
    }
    ;

mult_expr
    : unary_expr { $$ = $1; }
    | mult_expr MUL unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, $1, Decaf::IrBinaryOperator::Multiply, $3); 
    }
    | mult_expr DIV unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, $1, Decaf::IrBinaryOperator::Divide, $3); 
    }
    | mult_expr MOD unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, $1, Decaf::IrBinaryOperator::Modulo, $3); 
    }
    ;
    
add_expr
    : mult_expr { $$ = $1; }
    | add_expr PLUS mult_expr  
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, $1, Decaf::IrBinaryOperator::Add, $3); 
    }
    | add_expr MINUS mult_expr  
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, Decaf::IrType::Unknown, $1, Decaf::IrBinaryOperator::Subtract, $3); 
    }
    ;

rel_expr
    : add_expr { $$ = $1; }
    | rel_expr rel_op add_expr { $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, $1, (Decaf::IrBooleanOperator)$2, $3); }
    ;
 
logic_expr
    : rel_expr  { $$ = $1; }
    | logic_expr logic_op rel_expr  
    { 
        $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, $1, (Decaf::IrBooleanOperator)$2, $3); 
    }
    ;
    
assign_expr
    : logic_expr  { $$ = $1; }
    | assign_expr assign_op logic_expr
    {
        $$ = new Decaf::IrAssignExpression(@1.first_line, @1.first_column, $1, (Decaf::IrAssignmentOperator)$2, $3); 
    }
    ;

expr 
    : assign_expr { $$ = $1; }
    ;

expr_list 
    : expr { $$ = new std::vector<Decaf::IrExpression*>(); $$->push_back($1); }
    | expr_list COMMA expr { $$->push_back($3); }
    ;
    
rel_op 
    : CLT { $$ = (int)Decaf::IrBooleanOperator::Less; }
    | CGT { $$ = (int)Decaf::IrBooleanOperator::Greater; }
    | CLE { $$ = (int)Decaf::IrBooleanOperator::LessEqual; }
    | CGE { $$ = (int)Decaf::IrBooleanOperator::GreaterEqual; }
    | CEQ { $$ = (int)Decaf::IrBooleanOperator::Equal; }
    | CNE { $$ = (int)Decaf::IrBooleanOperator::NotEqual; }
    ;
    
logic_op 
    : LAND { $$ = (int)Decaf::IrBooleanOperator::LogicalAnd; }
    | LOR { $$ = (int)Decaf::IrBooleanOperator::LogicalOr; }
    ;
    
literal 
    : INTEGER 
    { 
        $$ = new Decaf::IrIntegerLiteral(@1.first_line, @1.first_column, d_scanner.matched()); 
    }
    | BOOLEAN 
    { 
        $$ = new Decaf::IrBooleanLiteral(@1.first_line, @1.first_column, d_scanner.matched()); 
    }
    | CHARACTER 
    { 
        $$ = new Decaf::IrCharacterLiteral(@1.first_line, @1.first_column, d_scanner.matched()); 
    }
    ;

string_literal
    : STRING
    {
        $$ = new Decaf::IrStringLiteral(@1.first_line, @1.first_column, d_scanner.matched());
    }
    ;