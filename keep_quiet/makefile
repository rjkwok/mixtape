EXE = debug
SFML_INCLUDE = C:/SFML-2.0/include
SFML_LIB = C:/SFML-2.0/lib

all: a.o b.o c.o d.o e.o f.o
	g++ -std=c++0x -static-libgcc -static-libstdc++ a.o b.o c.o d.o e.o f.o -o $(EXE) -L$(SFML_LIB) -L./lib -lsfml-network-d -lsfml-audio-d -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -ltinylib

a.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/main.cpp -o a.o

b.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/util.cpp -o b.o

c.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/ui.cpp -o c.o

d.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/structure.cpp -o d.o

e.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/terrain.cpp -o e.o

f.o:
	g++ -static-libgcc -static-libstdc++ -I$(SFML_INCLUDE) -std=c++0x -c source/collision.cpp -o f.o
