all: parser

CPPFLAGS = -std=c++11 -g -DDEBUG -Iir -I.
OBJS = parse.o lex.o main.o ir/IrClass.o ir/IrCommon.o ir/IrBlock.o ir/IrExprStmt.o ir/IrSymbolTable.o ir/IrIdentifier.o ir/IrFieldDecl.o ir/IrMethodDecl.o ir/IrVarDecl.o ir/IrMethodCall.o ir/IrLocation.o ir/IrBooleanExpr.o ir/IrBinaryExpr.o ir/IrIfStmt.o ir/IrForStmt.o ir/IrAssignExpr.o
LIBS = -lpopt

clean:
	$(RM) -rf parser Scannerbase.h parse.cc lex.cc Parserbase.h *.output $(OBJS)

lex.cc: decaf.l
	flexc++ --construction $^
	
Scannerbase.h: lex.cc

parse.cc: decaf.y
	bisonc++ --construction $^
	
Parser.h: parse.cc
Parserbase.h: parse.cc
Parser.ih: parse.cc
	
%.o: %.cpp %.cc
	g++ -c $(CPPFLAGS) -o $@ $<

parser: $(OBJS) Scanner.h
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: parser example.dcf
	cat example.dcf | ./parser --target parse
