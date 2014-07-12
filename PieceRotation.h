#ifndef PIECEROTATION_H
#define PIECEROTATION_H
#include "typedefs.h"
#include "RUL.h"
#include <stdlib.h>
#include <stdbool.h>

struct PieceRotation_s {
    NetworkFlags *network1;
    NetworkFlags *network2;
    Transformation transformation;
};

PieceRotation * pieceRotation_create(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south, Rotation rotation, Flip flip);
void pieceRotation_addNetwork2(PieceRotation *pr, NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south);
void pieceRotation_destroy(PieceRotation *pr);
PieceRotation * pieceRotation_clone(PieceRotation *pr);

//These functions just pass the PieceRotation through for convience.
PieceRotation * pieceRotation_rotate90C(PieceRotation *pr);
PieceRotation * pieceRotation_rotate90MC(PieceRotation *pr);
PieceRotation * pieceRotation_mirror(PieceRotation *pr);

void pieceRotation_print(PieceRotation *pr);

bool pieceRotation_fitsLeftOfPieceRotation(PieceRotation *pr, PieceRotation *pr2);
#endif
