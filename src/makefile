# Compiler and flags
CXX = g++
#CXXFLAGS = -std=c++11 -O2 -Wno-write-strings -static  # Add -static
CXXFLAGS = -std=c++17 -O3 -Ofast -flto -march=native -mtune=native -DNDEBUG -Wno-write-strings

# Source files
SRC = xboard.c vice.c uci.c evaluate.c pvtable.c init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c misc.c polybook.c polykeys.c nnue_eval.c nnue/nnue.cpp nnue/misc.cpp

# Output executable
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o vice