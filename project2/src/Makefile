all: parser

CPPFLAGS = -std=c++11
OBJS = parse.o lex.o main.o
LIBS = -lpopt

clean:
	$(RM) -rf parser Scannerbase.h parse.cc lex.cc Parserbase.h *.output $(OBJS)

lex.cc: decaf.lpp Scanner.ih Scanner.h
	flexc++ --construction $^
	
Scannerbase.h: lex.cc

parse.cc: decaf.g Scanner.h
	bisonc++ --construction --scanner=Scanner.h $^
	
Parser.h: parse.cc
Parserbase.h: parse.cc
Parser.ih: parse.cc
	
%.o: %.cpp %.cc
	g++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS) Scanner.h
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: parser example.dcf
	cat example.dcf | ./parser --target parse
