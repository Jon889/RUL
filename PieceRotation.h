#ifndef PIECEROTATION_H
#define PIECEROTATION_H
#include "NetworkFlags.h"
#include "RUL.h"
#include <stdlib.h>
#include <stdbool.h>

class PieceRotation {
public:
    NetworkFlags *network1;
    NetworkFlags *network2;
    NetworkFlags *override1;
    Transformation transformation;

    PieceRotation(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south, Rotation rotation, Flip flip);
    PieceRotation(NetworkFlags nf, Rotation rotation, Flip flip);
    void addNetwork2(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south);
    void addNetwork2(NetworkFlags nf);
    void addOverride1(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south);
    void addOverride1(NetworkFlags nf);
    ~PieceRotation();
    PieceRotation(PieceRotation *);

    //These functions just pass the PieceRotation through for convience.
    PieceRotation * rotate90C();
    PieceRotation * rotate90MC();
    PieceRotation * mirror();

    void print();

    bool fitsLeftOfPieceRotation(PieceRotation *pr2);
private:
    bool hasNetwork2();
};
#endif
