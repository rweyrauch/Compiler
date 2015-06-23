
%scanner Scanner.h
%lsp-needed 

%union {
    Decaf::IrProgram *programDef;
    Decaf::IrClass *classDef;
    Decaf::IrInterface *interfaceDef;
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
    Decaf::IrCaseStatement *caseStmt;
    std::vector<Decaf::IrCaseStatement*> *caseList;    
    Decaf::IrVariableDecl *varDecl;
    std::vector<Decaf::IrVariableDecl*> *varDeclList;
    Decaf::IrLiteral *literal;
    Decaf::IrIntegerLiteral *intLiteral;
    Decaf::IrStringLiteral *stringLiteral;
    std::string *string;
    int token;
}

%token RETURN CALLOUT
%token BOOLTYPE INTTYPE DOUBLETYPE STRINGTYPE CLASS VOID
%token IF SWITCH CASE DEFAULT FOR DO CONTINUE BREAK GOTO WHILE
%token INTERFACE NULLVALUE EXTENDS IMPLEMENTS THIS NEW

%token IDENTIFIER INTEGER BOOLEAN CHARACTER STRING DOUBLE
%token EQUAL PLUSEQUAL MINUSEQUAL
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA SEMI BANG COLON DOT
%token MOD LOR LAND

%token INCREMENT DECREMENT PLUS MINUS CEQ CNE CLT CLE CGT CGE 
%token MUL DIV

%nonassoc IFX
%nonassoc ELSE

%type <classDef> class
%type <interfaceDef> interface
%type <block> block
%type <methodDecl> method_decl method_signature
%type <methodDeclList> method_decl_list opt_method_decl_list
%type <fieldDeclList> field_decl
%type <fieldDeclListList> field_decl_list
%type <methodCall> method_call
%type <stmt> statement
%type <stmtList> statement_list opt_statement_list
%type <caseStmt> case_statement
%type <caseList> case_list
%type <varDecl> var_decl argument_decl
%type <varDeclList> var_decl_list argument_decl_list opt_argument_decl_list
%type <ident> ident qualified_ident
%type <identList> ident_list
%type <token> type rel_op logic_op assign_op
%type <literal> literal 
%type <intLiteral> int_literal
%type <stringLiteral> string_literal
%type <location> location
%type <locationList> location_list
%type <exprList> expr_list opt_expr_list
%type <expression> expr primary_expr unary_expr mult_expr add_expr rel_expr logic_expr assign_expr array_access array_access_list

%start program

%%

program
    : program_decl_list
    ;

program_decl_list
    : program_decl
    | program_decl_list program_decl
    ;

program_decl
    : class
    {
        getProgram()->addClassDecl(IrClassPtr($1));
    }
    | interface
    {
        getProgram()->addInterfaceDecl(IrInterfacePtr($1));
    }
    | field_decl
    {
        getProgram()->addFieldDecl(*$1);
    }
    | method_decl
    {
        getProgram()->addMethodDecl(IrMethodDeclPtr($1));
    }
    ;

class
    : CLASS ident LBRACE field_decl_list opt_method_decl_list RBRACE 
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2)); 
        for (auto it : *$4)
        {
            $$->addFieldDecl(it); 
        }
        if ($5 != nullptr)
        {
            $$->addMethodDecl(*$5); 
        }
    }
    | CLASS ident LBRACE opt_method_decl_list RBRACE  
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2)); 
        if ($4 != nullptr)
        {
            $$->addMethodDecl(*$4); 
        }
    }
    | CLASS ident EXTENDS ident LBRACE field_decl_list opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), IrIdentifierPtr($4)); 
        for (auto it : *$6)
        {
            $$->addFieldDecl(it); 
        }
        if ($7 != nullptr)
        {
            $$->addMethodDecl(*$7); 
        }
    }
    | CLASS ident EXTENDS ident LBRACE opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), IrIdentifierPtr($4)); 
        if ($6 != nullptr)
        {
            $$->addMethodDecl(*$6); 
        }
    }
    | CLASS ident IMPLEMENTS ident_list LBRACE field_decl_list opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), nullptr, $4); 
        for (auto it : *$6)
        {
            $$->addFieldDecl(it); 
        }
        if ($7 != nullptr)
        {
            $$->addMethodDecl(*$7); 
        }
    }     
    | CLASS ident IMPLEMENTS ident_list LBRACE opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), nullptr, $4); 
        if ($6 != nullptr)
        {
            $$->addMethodDecl(*$6); 
        }
    }     
    | CLASS ident EXTENDS ident IMPLEMENTS ident_list LBRACE field_decl_list opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), IrIdentifierPtr($4), $6); 
        for (auto it : *$8)
        {
            $$->addFieldDecl(it); 
        }
        if ($9 != nullptr)
        {
            $$->addMethodDecl(*$9); 
        }
    }     
    | CLASS ident EXTENDS ident IMPLEMENTS ident_list LBRACE opt_method_decl_list RBRACE
    { 
        $$ = new Decaf::IrClass(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), IrIdentifierPtr($4), $6); 
        if ($8 != nullptr)
        {
            $$->addMethodDecl(*$8); 
        }
    }     
    ;
    
interface
    : INTERFACE ident LBRACE opt_method_prototype_list RBRACE
    {
        $$ = new Decaf::IrInterface(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2)); 
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
            Decaf::IrLocationPtr location(*it); 
            location->setAsDeclaration();
            Decaf::IrFieldDecl* decl = new Decaf::IrFieldDecl(@1.first_line, @1.first_column, d_scanner.filename(), location, (Decaf::IrType)$1); 
            $$->push_back(decl); 
        } 
    }
    | ident location_list SEMI
    {
        $$ = new std::vector<Decaf::IrFieldDecl*>(); 
        for (auto it = $2->begin(); it != $2->end(); ++it) 
        { 
            Decaf::IrLocationPtr location(*it); 
            location->setAsDeclaration();
            Decaf::IrFieldDecl* decl = new Decaf::IrFieldDecl(@1.first_line, @1.first_column, d_scanner.filename(), location, IrIdentifierPtr($1)); 
            $$->push_back(decl); 
        } 
    }
    | ident LBRACKET RBRACKET location_list SEMI
    {
        $$ = new std::vector<Decaf::IrFieldDecl*>(); 
        for (auto it = $4->begin(); it != $4->end(); ++it) 
        { 
            Decaf::IrLocationPtr location(*it); 
            location->setAsDeclaration();
            Decaf::IrFieldDecl* decl = new Decaf::IrFieldDecl(@1.first_line, @1.first_column, d_scanner.filename(), location, IrIdentifierPtr($1)); 
            $$->push_back(decl); 
        } 
    }
    ;

opt_method_decl_list
    : // empty
    {
        $$ = nullptr;
    }
    | method_decl_list
    {
        $$ = $1;
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
    
opt_argument_decl_list
    : // empty
    {
        $$ = nullptr;
    }
    | argument_decl_list
    {
        $$ = $1;
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
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), (Decaf::IrType)$1); 
    }
    | ident ident 
    { 
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), IrIdentifierPtr($1)); 
    }
    | ident LBRACKET RBRACKET ident 
    { 
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($4), IrIdentifierPtr($1)); 
    }
    ;
    
method_signature
    : type ident LPAREN opt_argument_decl_list RPAREN 
    { 
        $$ = new Decaf::IrMethodDecl(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), (Decaf::IrType)$1);
        if ($4 != nullptr)
        {
            for (auto it : *$4)
            {
                $$->addArgument(IrVariableDeclPtr(it));
            }
        }
    }
    | ident ident LPAREN opt_argument_decl_list RPAREN
    { 
        $$ = new Decaf::IrMethodDecl(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), Decaf::IrType::Class); 
        if ($4 != nullptr)
        {
            for (auto it : *$4)
            {
                $$->addArgument(IrVariableDeclPtr(it));
            }
        }
    }
    ;

method_decl 
    : method_signature block
    {
        $1->addBlock(IrBlockPtr($2));
    }
    ;

opt_method_prototype_list
    : // empty
    | method_prototype_list
    ;

method_prototype_list
    : method_prototype
    | method_prototype_list method_prototype
    ;

method_prototype
    : method_signature SEMI
    ;

block 
    : LBRACE var_decl_list opt_statement_list RBRACE 
    { 
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column, d_scanner.filename()); 
        $$->addVariableDecl(*$2); 
        if ($3 != nullptr)
        {
            $$->addStatements(*$3); 
        }
    }
    | LBRACE opt_statement_list RBRACE 
    {
        $$ = new Decaf::IrBlock(@1.first_line, @1.first_column, d_scanner.filename()); 
        if ($2 != nullptr)
        {
            $$->addStatements(*$2); 
        }
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
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), *$2, (Decaf::IrType)$1); 
    }
    | ident ident_list SEMI
    {
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), *$2, IrIdentifierPtr($1)); 
    }
    | ident LBRACKET RBRACKET ident_list SEMI
    {
        $$ = new Decaf::IrVariableDecl(@1.first_line, @1.first_column, d_scanner.filename(), *$4, IrIdentifierPtr($1)); 
    }
    ;

case_list
    : case_statement
    {
        $$ = new std::vector<Decaf::IrCaseStatement*>();
        $$->push_back($1);
    }
    | case_list case_statement
    {
        $1->push_back($2);
    }
    ;

case_statement
    : CASE int_literal COLON opt_statement_list
    {
        $$ = new Decaf::IrCaseStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrIntegerLiteralPtr($2));
        if ($4 != nullptr)
        {
            $$->addStatements(*$4); 
        }
    }
    | DEFAULT COLON opt_statement_list
    {
        $$ = new Decaf::IrCaseStatement(@1.first_line, @1.first_column, d_scanner.filename());
        if ($3 != nullptr)
        {
            $$->addStatements(*$3); 
        }
    }
    ;

opt_statement_list
    : // empty
    {
        $$ = nullptr;
    }
    | statement_list
    {
        $$ = $1;
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
    | DOUBLETYPE
    {
        $$ = (int)Decaf::IrType::Double;
    }
    | STRINGTYPE
    {
        $$ = (int)Decaf::IrType::String;
    }
    | VOID 
    { 
        $$ = (int)Decaf::IrType::Void; 
    }
    | type LBRACKET RBRACKET
    {
        $$ = (int)Decaf::IrType::Array;
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
   
array_access 
    : LBRACKET expr RBRACKET
    {
        $$ = $2;
    }
    ;
    
array_access_list
    : array_access
    {
        $$ = $1;
    }
    | array_access_list array_access
    {
        $$ = $2;
    }
    ;
 
qualified_ident
    : ident DOT ident
    {
        $$ = $1;
    }
    | qualified_ident DOT ident
    {
        $$ = $1;
    }
    ;

ident 
    : IDENTIFIER 
    { 
        $$ = new Decaf::IrIdentifier(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched()); 
    }
    | THIS 
    { 
        $$ = new Decaf::IrIdentifier(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched()); // create IrThisIdentifier();
    }
    ;

statement 
    : IF LPAREN expr RPAREN statement %prec IFX
    { 
        $$ = new Decaf::IrIfStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($3), IrStatementPtr($5)); 
    }
    | IF LPAREN expr RPAREN statement ELSE statement  
    { 
        $$ = new Decaf::IrIfStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($3), IrStatementPtr($5), IrStatementPtr($7)); 
    }
    | FOR LPAREN expr SEMI expr SEMI expr RPAREN statement 
    { 
        $$ = new Decaf::IrForStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($3), IrExpressionPtr($5), IrExpressionPtr($7), IrStatementPtr($9)); 
    }
    | WHILE LPAREN expr RPAREN statement
    {
        $$ = new Decaf::IrWhileStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($3), IrStatementPtr($5)); 
    }
    | DO statement WHILE LPAREN expr RPAREN
    {
        $$ = new Decaf::IrDoWhileStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($5), IrStatementPtr($2)); 
    }
    | SWITCH LPAREN expr RPAREN LBRACE case_list RBRACE
    {
        Decaf::IrSwitchStatement* stmt = new Decaf::IrSwitchStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($3));
        stmt->addStatements(*$6); 
        $$ = stmt;
    }
    | RETURN SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(@1.first_line, @1.first_column, d_scanner.filename()); 
    }
    | RETURN expr SEMI 
    { 
        $$ = new Decaf::IrReturnStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($2)); 
    }
    | BREAK SEMI 
    { 
        $$ = new Decaf::IrBreakStatement(@1.first_line, @1.first_column, d_scanner.filename()); 
    }
    | CONTINUE SEMI 
    {   
        $$ = new Decaf::IrContinueStatement(@1.first_line, @1.first_column, d_scanner.filename()); 
    }
    | expr SEMI 
    { 
        $$ = new Decaf::IrExpressionStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($1)); 
    }
    | GOTO ident SEMI
    {
        $$ = new Decaf::IrGotoStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2));
    }
    | ident COLON
    {
        $$ = new Decaf::IrLabelStatement(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1));
    }
    | block 
    { 
        $$ = $1; 
    }
    ;
    
assign_op 
    : EQUAL 
    { 
        $$ = (int)Decaf::IrAssignmentOperator::Assign; 
    }
    | PLUSEQUAL 
    { 
        $$ = (int)Decaf::IrAssignmentOperator::IncrementAssign; 
    }
    | MINUSEQUAL 
    { 
        $$ = (int)Decaf::IrAssignmentOperator::DecrementAssign; 
    }
    ;
    
method_call 
    : ident LPAREN opt_expr_list RPAREN 
    { 
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown);
        if ($3 != nullptr)
        {
            for (auto it : *$3) 
            {
                $$->addArgument(IrExpressionPtr(it));
            }
        }
    }
    | qualified_ident LPAREN opt_expr_list RPAREN
    {
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown);
        if ($3 != nullptr)
        {
            for (auto it : *$3) 
            {
                $$->addArgument(IrExpressionPtr(it));
            }
        }
    }
    | CALLOUT LPAREN string_literal COMMA expr_list RPAREN
    {
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrStringLiteralPtr($3), Decaf::IrType::Integer);
        for (auto it : *$5) 
        {
            $$->addArgument(IrExpressionPtr(it));
        }
    }
    | CALLOUT LPAREN string_literal RPAREN
    {
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrStringLiteralPtr($3), Decaf::IrType::Integer);
    }
    | NEW ident 
    { 
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), Decaf::IrType::Unknown); // create IrNewCall();
    }
    | NEW type array_access
    { 
        $$ = nullptr; // create IrNewCall();
    }
    | NEW ident array_access
    { 
        $$ = new Decaf::IrMethodCall(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($2), Decaf::IrType::Array); // create IrNewCall();
    }
    ;

location 
    : ident 
    { 
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown); 
    }
    | ident array_access_list 
    { 
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown, IrExpressionPtr($2)); 
    }
    | qualified_ident
    {
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown);     
    }
    | ident array_access_list DOT ident
    {
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown, IrExpressionPtr($2));     
    }
    | qualified_ident array_access_list
    {
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown);     
    }
    | ident array_access_list DOT ident array_access_list
    {
        $$ = new Decaf::IrLocation(@1.first_line, @1.first_column, d_scanner.filename(), IrIdentifierPtr($1), Decaf::IrType::Unknown, IrExpressionPtr($2));     
    }
    ;

primary_expr
    : location 
    { 
        $$ = $1; 
    }
    | literal 
    { 
        $$ = $1; 
    }
    | string_literal 
    { 
        $$ = $1; 
    }
    | method_call 
    { 
        $$ = $1; 
    }
    | LPAREN expr RPAREN 
    { 
        $$ = $2; 
    }
    ;
    
unary_expr
    : primary_expr 
    { 
        $$ = $1; 
    }
    | MINUS unary_expr 
    { 
        Decaf::IrIntegerLiteral* intLit = dynamic_cast<Decaf::IrIntegerLiteral*>($2);
        Decaf::IrDoubleLiteral* doubleLit = dynamic_cast<Decaf::IrDoubleLiteral*>($2);
        if (intLit)
        {
            // convert expression into negative integer literal
            intLit->setValue(-intLit->getValue());            
            $$ = $2;
        }
        else if (doubleLit)
        {
            // convert expression into negative double literal
            doubleLit->setValue(-doubleLit->getValue());            
            $$ = $2;
        }
        else
        {
            $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, nullptr, Decaf::IrBinaryOperator::Subtract, IrExpressionPtr($2)); 
        }
    }
    | BANG unary_expr  
    { 
        Decaf::IrBooleanLiteral* boolLit = dynamic_cast<Decaf::IrBooleanLiteral*>($2);
        if (boolLit == nullptr)
        {
            $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, d_scanner.filename(), nullptr, Decaf::IrBooleanOperator::Not, IrExpressionPtr($2)); 
        }
        else
        {
            // negate boolean literal
            boolLit->setValue(!boolLit->getValue());
            $$ = $2;
        }
    }
    ;

mult_expr
    : unary_expr 
    { 
        $$ = $1; 
    }
    | mult_expr MUL unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, IrExpressionPtr($1), Decaf::IrBinaryOperator::Multiply, IrExpressionPtr($3)); 
    }
    | mult_expr DIV unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, IrExpressionPtr($1), Decaf::IrBinaryOperator::Divide, IrExpressionPtr($3)); 
    }
    | mult_expr MOD unary_expr 
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, IrExpressionPtr($1), Decaf::IrBinaryOperator::Modulo, IrExpressionPtr($3)); 
    }
    ;
    
add_expr
    : mult_expr 
    { 
        $$ = $1; 
    }
    | add_expr PLUS mult_expr  
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, IrExpressionPtr($1), Decaf::IrBinaryOperator::Add, IrExpressionPtr($3)); 
    }
    | add_expr MINUS mult_expr  
    { 
        $$ = new Decaf::IrBinaryExpression(@1.first_line, @1.first_column, d_scanner.filename(), Decaf::IrType::Unknown, IrExpressionPtr($1), Decaf::IrBinaryOperator::Subtract, IrExpressionPtr($3)); 
    }
    ;

rel_expr
    : add_expr 
    { 
        $$ = $1; 
    }
    | rel_expr rel_op add_expr 
    { 
        $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($1), (Decaf::IrBooleanOperator)$2, IrExpressionPtr($3)); 
    }
    ;
 
logic_expr
    : rel_expr  
    { 
        $$ = $1; 
    }
    | logic_expr logic_op rel_expr  
    { 
        $$ = new Decaf::IrBooleanExpression(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($1), (Decaf::IrBooleanOperator)$2, IrExpressionPtr($3)); 
    }
    ;
    
assign_expr
    : logic_expr  
    { 
        $$ = $1; 
    }
    | assign_expr assign_op logic_expr
    {
        $$ = new Decaf::IrAssignExpression(@1.first_line, @1.first_column, d_scanner.filename(), IrExpressionPtr($1), (Decaf::IrAssignmentOperator)$2, IrExpressionPtr($3)); 
    }
    ;

expr 
    : assign_expr 
    { 
        $$ = $1; 
    }
    ;

opt_expr_list
    : // empty
    {
        $$ = nullptr;
    }
    | expr_list
    {
        $$ = $1;
    }
    ;

expr_list 
    : expr 
    { 
        $$ = new std::vector<Decaf::IrExpression*>(); $$->push_back($1); 
    }
    | expr_list COMMA expr 
    { 
        $$->push_back($3); 
    }
    ;
    
rel_op 
    : CLT 
    { 
        $$ = (int)Decaf::IrBooleanOperator::Less; 
    }
    | CGT 
    { 
        $$ = (int)Decaf::IrBooleanOperator::Greater; 
    }
    | CLE 
    { 
        $$ = (int)Decaf::IrBooleanOperator::LessEqual; 
    }
    | CGE 
    { 
        $$ = (int)Decaf::IrBooleanOperator::GreaterEqual; 
    }
    | CEQ 
    { 
        $$ = (int)Decaf::IrBooleanOperator::Equal; 
    }
    | CNE 
    { 
        $$ = (int)Decaf::IrBooleanOperator::NotEqual; 
    }
    ;
    
logic_op 
    : LAND 
    { 
        $$ = (int)Decaf::IrBooleanOperator::LogicalAnd; 
    }
    | LOR 
    { 
        $$ = (int)Decaf::IrBooleanOperator::LogicalOr; 
    }
    ;

int_literal
    : INTEGER
    {
        $$ = new Decaf::IrIntegerLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched()); 
    }
    ;
    
literal 
    : int_literal
    {
        $$ = $1;
    }
    | BOOLEAN 
    { 
        $$ = new Decaf::IrBooleanLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched()); 
    }
    | CHARACTER 
    { 
        $$ = new Decaf::IrCharacterLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched()); 
    }
    | DOUBLE
    {
        $$ = new Decaf::IrDoubleLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched());
    }
    | NULLVALUE
    {
        $$ = new Decaf::IrNullLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched());
    }
    ;

string_literal
    : STRING
    {
        $$ = new Decaf::IrStringLiteral(@1.first_line, @1.first_column, d_scanner.filename(), d_scanner.matched());
    }
    ;
