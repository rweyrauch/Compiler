all: parser

OBJS = decafTokens.o decafParser.o main.o

clean:
	$(RM) -rf parser decafTokens.cpp decafParser.cpp decafParser.hpp decafParser.output $(OBJS)

decafParser.cpp: decaf.y
	bison --defines --report=all --warnings=all --output=$@ $^
	
decafParser.hpp: decafParser.cpp

decafTokens.cpp: decaf.lex decafParser.hpp
	flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: parser example.d
	cat example.d | ./parser
