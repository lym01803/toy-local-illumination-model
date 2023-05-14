.PHONY: all, clean

all: main 

main: ./src/main.o ./src/loadshader.o ./src/loadobj.o
	g++ -o $@ $+ -L${PWD}/lib -lglfw3 -lglad -framework Cocoa -framework OpenGL -framework IOKit 

./src/main.o: ./src/main.cpp
	g++ -c -std=c++17 -o $@ -I${PWD}/include $+

./src/loadshader.o: ./src/loadshader.cpp 
	g++ -c -std=c++17 -o $@ -I${PWD}/include $+

./src/loadobj.o: ./src/loadobj.cpp
	g++ -c -std=c++17 -o $@ -I${PWD}/include $+

clean:
	rm -f main
	rm -f ./src/*.o
