#include "PieceRotation.h"
#include "NetworkFlags.h"
#include <stdlib.h>
#include <stdio.h>

PieceRotation * pieceRotation_create(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south, Rotation rotation, Flip flip) {
    PieceRotation *p = calloc(1, sizeof(PieceRotation));
    p->network1 = networkFlags_create(west, north, east, south);
    p->override1 = networkFlags_create(west, north, east, south);
    p->network2 = networkFlags_create(0,0,0,0);
    p->transformation.rotation = rotation;
    p->transformation.flip = flip;
    return p;
}
void pieceRotation_addNetwork2(PieceRotation *pr, NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    networkFlags_destroy(pr->network2);
    pr->network2 = networkFlags_create(west, north, east, south);
}

PieceRotation * pieceRotation_create_s(NetworkFlags nf, Rotation rotation, Flip flip) {
    return pieceRotation_create(nf.west, nf.north, nf.east, nf.south, rotation, flip);
}
void pieceRotation_addNetwork2_s(PieceRotation *pr, NetworkFlags nf) {
    pieceRotation_addNetwork2(pr, nf.west, nf.north, nf.east, nf.south);
}
void pieceRotation_addOverride1(PieceRotation *pr, NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    networkFlags_destroy(pr->override1);
    pr->override1 = networkFlags_create(west, north, east, south);
}
void pieceRotation_addOverride1_s(PieceRotation *pr, NetworkFlags nf) {
    pieceRotation_addOverride1(pr, nf.west, nf.north, nf.east, nf.south);
}


void pieceRotation_destroy(PieceRotation *pr) {
    networkFlags_destroy(pr->network1);
    networkFlags_destroy(pr->network2);
    networkFlags_destroy(pr->override1);
    free(pr);
}
PieceRotation * pieceRotation_clone(PieceRotation *pr) {
    PieceRotation *npr = pieceRotation_create_s(*pr->network1, pr->transformation.rotation, pr->transformation.flip);
    pieceRotation_addOverride1_s(npr, *pr->override1);
    pieceRotation_addNetwork2_s(npr, *pr->network2);
    return npr;
}
PieceRotation * pieceRotation_rotate90C(PieceRotation *pr) {
    networkFlags_rotate90C(pr->network1);
    networkFlags_rotate90C(pr->network2);
    networkFlags_rotate90C(pr->override1);
    Rotation *r = &pr->transformation.rotation;
    if (*r == Rotation_270) {
        *r = Rotation_90;
    } else {
        (*r)++;
    }
    return pr;
}
PieceRotation * pieceRotation_rotate90MC(PieceRotation *pr) {
    networkFlags_rotate90MC(pr->network1);
    networkFlags_rotate90MC(pr->network2);
    networkFlags_rotate90MC(pr->override1);
    Rotation *r = &pr->transformation.rotation;
    if (*r == Rotation_270) {
        *r = Rotation_90;
    } else {
        (*r)++;
    }
    return pr;
}
PieceRotation * pieceRotation_mirror(PieceRotation *pr) {
    networkFlags_mirror(pr->network1);
    networkFlags_mirror(pr->network2);
    networkFlags_mirror(pr->override1);
    pr->transformation.flip = pr->transformation.flip == 0 ? 1 : 0;
    return pr;
}

bool pieceRotation_hasNetwork2(PieceRotation *pr) {
    return (pr->network2->west == 0
            && pr->network2->north == 0
            && pr->network2->east == 0
            && pr->network2->south == 0);
}

bool pieceRotation_fitsLeftOfPieceRotation(PieceRotation *pr, PieceRotation *pr2) {
    if (networkFlag_matches(pr->network1->east, pr2->network1->west, false)) {
        if ((!pieceRotation_hasNetwork2(pr) && !pieceRotation_hasNetwork2(pr2))
            || ((pieceRotation_hasNetwork2(pr) && pieceRotation_hasNetwork2(pr2)
                && networkFlag_matches(pr->network2->east, pr2->network2->west, true))) {
            //C4|C4
//            if (pr2->override1->west != pr2->network1->west
//                && pr->override1->east != pr2->override1->west) {
//                return false;
//            }
            return true;
        }
    }
    return false;
}
void pieceRotation_print(PieceRotation *pr) {
    printf("Network 1: %02i %02i %02i %02i\n", pr->network1->west, pr->network1->north, pr->network1->east, pr->network1->south);
    if (pieceRotation_hasNetwork2(pr)) {
        printf("Network 2: %02i %02i %02i %02i\n", pr->network2->west, pr->network2->north, pr->network2->east, pr->network2->south);
    }
    printf("Rotation: %i, Flip: %i\n", pr->transformation.rotation, pr->transformation.flip);
}

