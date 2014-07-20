CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -pedantic 

.SUFFIXES: .c .o

.PHONY: all clean

all: RUL

RUL: RUL.o List.o Piece.o NetworkFlags.o PieceRotation.o

List.o: List.c List.h typedefs.h
NetworkFlags.o: NetworkFlags.c NetworkFlags.h RUL.h typedefs.h
Piece.o: Piece.c Piece.h RUL.h typedefs.h List.h PieceRotation.h \
  NetworkFlags.h
PieceRotation.o: PieceRotation.c PieceRotation.h typedefs.h RUL.h \
  NetworkFlags.h
RUL.o: RUL.c RUL.h typedefs.h List.h Piece.h PieceRotation.h \
  NetworkFlags.h

clean:
	rm -f $(wildcard *.o)
	rm -f RUL
