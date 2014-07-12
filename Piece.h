#ifndef PIECE_H
#define PIECE_H
#include <stdbool.h>
#include "RUL.h"
#include "Map.h"
#include "typedefs.h"



struct Piece_s {
    PieceRotation *rotations[8];
    InstanceID textureIID;
    char *name;
    bool simple;
};



Piece * piece_create(InstanceID tid, char *name, bool simple);
Piece * piece_createEmpty(void);
void piece_fillRotationsFromBase(Piece *piece, PieceRotation *baseRotation);
void piece_destroy(Piece *piece);
Piece * piece_clone(Piece *piece);

void piece_printAsciiArt(Piece *p, int rotation);
void piece_printAsciiArtPair(Piece *piece, int rotation1, Piece *p2, int rotation2);
int piece_printRules(Piece *leftPiece, Map *map, List *list);
void piece_printRULs(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri);
int piece_printRulesOfPieces(Piece *leftPiece, Piece *rightPiece, Piece *rightOverridePiece, bool asciiArt, bool printFailures);
#endif
