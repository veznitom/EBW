OUT = SimpleTableEditor

all:
	g++ -std=c++14 -Wall -pedantic -fsanitize=address ebw/*.cpp -g -lncurses
compile:
	g++ -std=c++14 -Wall -pedantic -fsanitize=address ebw/*.cpp -o ./$(OUT) -g -lncurses
run:
	./$(OUT)
clean:
	
doc:
	doxygen Doxyfile
