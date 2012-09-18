CC=g++
CFLAGS=-std=c++11 -Wall

stdafx.h.gch : stdafx.h
	$(CC) $(CFLAGS) -c stdafx.h -o stdafx.h.gch

roguelike.o: roguelike.cpp
	$(CC) $(CFLAGS) -c roguelike.cpp -o roguelike.o

main: roguelike.o stdafx.h.gch
	$(CC) roguelike.o -lsfml-graphics -lsfml-window -lsfml-system -o roguelike
