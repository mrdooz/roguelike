CC=clang
CFLAGS=-std=c++11 -I/opt/local/include

precompiled.h.gch : precompiled.hpp
	$(CC) $(CFLAGS) -c precompiled.hpp -o precompiled.hpp.gch

roguelike.o: roguelike.cpp
	$(CC) $(CFLAGS) -c roguelike.cpp -o roguelike.o

main: roguelike.o precompiled.hpp.gch
	$(CC) roguelike.o -libstdc++ -lsfml-graphics -lsfml-window -lsfml-system -o roguelike
