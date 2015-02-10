all: parser

CPPFLAGS = -std=c++11
OBJS = parse.o lex.o main.o
LIBS = -lpopt

clean:
	$(RM) -rf parser Scanner.h Scannerbase.h Scanner.ih parse.cc lex.cc Parser.h Parserbase.h Parser.ih $(OBJS)

parse.cc: decaf.g Scanner.h
	bisonc++ --scanner=Scanner.h $^
	
Parser.h: parse.cc
Parserbase.h: parse.cc
Parser.ih: parse.cc

Scanner.h: decaf.lpp
	flexc++ $^

%.o: %.cpp %.cc
	g++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: parser example.d
	cat example.d | ./parser --target parse
