CC      = g++
CXXFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c++14 -pedantic

.SUFFIXES: .c .cpp .o

.PHONY: all clean

all: RUL

RUL: RUL.o List.o Piece.o NetworkFlags.o PieceRotation.o

List.o: List.cpp List.h typedefs.h
NetworkFlags.o: NetworkFlags.cpp NetworkFlags.h RUL.h typedefs.h
Piece.o: Piece.cpp Piece.h RUL.h typedefs.h List.h PieceRotation.h \
  NetworkFlags.h
PieceRotation.o: PieceRotation.cpp PieceRotation.h typedefs.h RUL.h \
  NetworkFlags.h
RUL.o: RUL.cpp RUL.h typedefs.h List.h Piece.h PieceRotation.h \
  NetworkFlags.h

clean:
	rm -f $(wildcard *.o)
	rm -f RUL
