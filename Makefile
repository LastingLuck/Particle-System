CC=g++
CFLAGS=-std=c++11 -g -Wall -fopenmp
LFLAGS=-lSDL2 -lGL -lGLEW
DFLAG=
ifdef DEBUG
	DFLAG+=-DDEBUG=1
endif

all: particles staticParticles windParticles smoke

particles: main.o Particle.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) $^ -o $@

staticParticles: main2.o Particle.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) $^ -o $@

windParticles: main3.o Particle.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) $^ -o $@

smoke: main4.o Particle.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

main2.o: main2.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

main3.o: main3.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

main4.o: main4.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

Particle.o: Particle.cpp Particle.h
	$(CC) $(CFLAGS) $(LFLAGS) $(DFLAG) -c $^

clean:
	rm -rf *.o particles staticParticles windParticles smoke