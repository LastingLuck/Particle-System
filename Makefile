CC=g++
CFLAGS=-std=c++11 -g -Wall
LFLAGS=-lSDL2 -lGL -lGLEW
DFLAG=
ifdef DEBUG
	DFLAG+=-DDEBUG=1
endif

all: particles

particles: main.o Particle.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

Particle.o: Particle.cpp Particle.h
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

clean:
	rm -rf *.o particles