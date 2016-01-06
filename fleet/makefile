EXE = debug
SFML_INCLUDE = C:/SFML-2.0/include
SFML_LIB = C:/SFML-2.0/lib

all: main.o util.o ui.o round.o
	g++ -std=c++0x -static-libgcc -static-libstdc++ main.o util.o ui.o round.o -o $(EXE) -L$(SFML_LIB) -L./lib -lsfml-network -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -ltinylib

main.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c main.cpp

util.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c util.cpp
	
ui.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c ui.cpp 

round.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c round.cpp

