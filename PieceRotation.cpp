#include "PieceRotation.h"
#include "NetworkFlags.h"
#include <stdlib.h>
#include <stdio.h>

PieceRotation::PieceRotation(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south, Rotation rotation, Flip flip) {
    network1 = new NetworkFlags(west, north, east, south);
    override1 = new NetworkFlags(west, north, east, south);
    network2 = new NetworkFlags();
    transformation.rotation = rotation;
    transformation.flip = flip;
}

PieceRotation::PieceRotation(NetworkFlags nf, Rotation rotation, Flip flip) : PieceRotation(nf.west, nf.north, nf.east, nf.south, rotation, flip) {}

void PieceRotation::addNetwork2(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    delete network2;
    network2 = new NetworkFlags(west, north, east, south);
}

void PieceRotation::addNetwork2(NetworkFlags nf) {
    addNetwork2(nf.west, nf.north, nf.east, nf.south);
}

void PieceRotation::addOverride1(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    delete override1;
    override1 = new NetworkFlags(west, north, east, south);
}

void PieceRotation::addOverride1(NetworkFlags nf) {
    addOverride1(nf.west, nf.north, nf.east, nf.south);
}

PieceRotation::~PieceRotation() {
    delete network1;
    delete network2;
    delete override1;
}
PieceRotation::PieceRotation(PieceRotation *pr) : PieceRotation(*pr->network1, pr->transformation.rotation, pr->transformation.flip) {
    addOverride1(*pr->override1);
    addNetwork2(*pr->network2);
}
PieceRotation * PieceRotation::rotate90C() {
    network1->rotate90C();
    network2->rotate90C();
    override1->rotate90C();
    Rotation *r = &transformation.rotation;
    if (*r == Rotation_270) {
        *r = Rotation_90;
    } else {
        (*r)++;
    }
    return this;
}
PieceRotation * PieceRotation::rotate90MC() {
    network1->rotate90MC();
    network2->rotate90MC();
    override1->rotate90MC();
    Rotation *r = &transformation.rotation;
    if (*r == Rotation_270) {
        *r = Rotation_90;
    } else {
        (*r)++;
    }
    return this;
}
PieceRotation * PieceRotation::mirror() {
    network1->mirror();
    network2->mirror();
    override1->mirror();
    transformation.flip = transformation.flip == 0 ? 1 : 0;
    return this;
}

bool PieceRotation::hasNetwork2() {
    return (network2->west == 0
            && network2->north == 0
            && network2->east == 0
            && network2->south == 0);
}

bool PieceRotation::fitsLeftOfPieceRotation(PieceRotation *pr2) {
    if (networkFlag_matches(network1->east, pr2->network1->west, false)) {
        if ((!hasNetwork2() && !pr2->hasNetwork2())
            || ((hasNetwork2() && pr2->hasNetwork2()
                && networkFlag_matches(network2->east, pr2->network2->west, true)))) {
            //C4|C4
//            if (pr2->override1->west != pr2->network1->west
//                && override1->east != pr2->override1->west) {
//                return false;
//            }
            return true;
        }
    }
    return false;
}
void PieceRotation::print() {
    printf("Network 1: %02i %02i %02i %02i\n", network1->west, network1->north, network1->east, network1->south);
    if (hasNetwork2()) {
        printf("Network 2: %02i %02i %02i %02i\n", network2->west, network2->north, network2->east, network2->south);
    }
    printf("Rotation: %i, Flip: %i\n", transformation.rotation, transformation.flip);
}

