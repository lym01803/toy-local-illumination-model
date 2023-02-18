.PHONY: all, clean

all: main 

main: ./src/main.o ./src/loadshader.o
	g++ -o $@ $+ -L${PWD}/lib -lglfw3 -lglad -framework Cocoa -framework OpenGL -framework IOKit 

./src/main.o: ./src/main.cpp
	g++ -c -o $@ -I${PWD}/include $+

./src/loadshader.o: ./src/loadshader.cpp 
	g++ -c -o $@ -I${PWD}/include $+

clean:
	rm -f main
	rm -f ./src/*.o
