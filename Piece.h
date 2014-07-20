#ifndef PIECE_H
#define PIECE_H
#include <stdbool.h>
#include "RUL.h"
#include "typedefs.h"
#include "List.h"


struct Piece_s {
    PieceRotation *rotations[8];
    InstanceID textureIID;
    char *name;
    bool simple;
    char *network1Name;
    char *network2Names[10];
};



Piece * piece_create(InstanceID tid, char *name, bool simple);
Piece * piece_createEmpty(void);
void piece_fillRotationsFromBase(Piece *piece, PieceRotation *baseRotation);
void piece_destroy(Piece *piece);
Piece * piece_clone(Piece *piece);
Piece *list_findPieceWithNetworkFlags(List *list, char *n1name, NetworkFlags *n1flags, char *n2names[10], NetworkFlags *n2flags, Rotation *rot);
void piece_printAsciiArt(Piece *p, int rotation);
void piece_printAsciiArtPair(Piece *piece, int rotation1, Piece *p2, int rotation2);
int piece_printRules(Piece *leftPiece, List *list);
void piece_printRULs(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri);
int piece_printRulesOfPieces(Piece *leftPiece, Piece *rightPiece, Piece *rightOverridePiece, bool asciiArt, bool printFailures);
bool piece_printRUL(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri);
#endif
