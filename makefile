OUTPUT_NAME=2048-game
CXXFLAGS=-g
LNFLAGS=-lncurses

all:
	g++ $(LNFLAGS) $(CXXFLAGS) main.cpp -o $(OUTPUT_NAME) -O3