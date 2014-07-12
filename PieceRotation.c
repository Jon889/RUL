#include "PieceRotation.h"
#include "NetworkFlags.h"
#include <stdlib.h>
#include <stdio.h>

PieceRotation * pieceRotation_create(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south, Rotation rotation, Flip flip) {
    PieceRotation *p = calloc(1, sizeof(PieceRotation));
    p->network1 = networkFlags_create(west, north, east, south);
    p->network2 = networkFlags_create(0,0,0,0);
    p->transformation.rotation = rotation;
    p->transformation.flip = flip;
    return p;
}
void pieceRotation_addNetwork2(PieceRotation *pr, NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    networkFlags_destroy(pr->network2);
    pr->network2 = networkFlags_create(west, north, east, south);
}
void pieceRotation_destroy(PieceRotation *pr) {
    networkFlags_destroy(pr->network1);
    networkFlags_destroy(pr->network2);
    free(pr);
}
PieceRotation * pieceRotation_clone(PieceRotation *pr) {
    PieceRotation *npr = pieceRotation_create(pr->network1->west,
                                              pr->network1->north,
                                              pr->network1->east,
                                              pr->network1->south,
                                              pr->transformation.rotation,
                                              pr->transformation.flip);
    if (pr->network2 != NULL) {
        pieceRotation_addNetwork2(npr, pr->network2->west,
                                  pr->network2->north,
                                  pr->network2->east,
                                  pr->network2->south);
    }
    return npr;
}
PieceRotation * pieceRotation_rotate90C(PieceRotation *pr) {
    networkFlags_rotate90C(pr->network1);
    if (pr->network2 != NULL) {
        networkFlags_rotate90C(pr->network2);
    }
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
    if (pr->network2 != NULL) {
        networkFlags_rotate90MC(pr->network2);
    }
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
    if (pr->network2 != NULL) {
        networkFlags_mirror(pr->network2);
    }
    pr->transformation.flip = pr->transformation.flip == 0 ? 1 : 0;
    return pr;
}


bool pieceRotation_fitsLeftOfPieceRotation(PieceRotation *pr, PieceRotation *pr2) {
    if (networkFlags_fitsLeftOfNetworkFlags(pr->network1, pr2->network1, false)) {
        if (pr->network2 == NULL && pr2->network2 == NULL) {
            return true;
        } else if (pr->network2 != NULL && pr2->network2 != NULL
                   && networkFlags_fitsLeftOfNetworkFlags(pr->network2, pr2->network2, true)) {
            return true;
        }
    }
    return false;
}
void pieceRotation_print(PieceRotation *pr) {
    printf("Network 1: %02i %02i %02i %02i\n", pr->network1->west, pr->network1->north, pr->network1->east, pr->network1->south);
    if (pr->network2 != NULL) {
        printf("Network 2: %02i %02i %02i %02i\n", pr->network2->west, pr->network2->north, pr->network2->east, pr->network2->south);
    }
    printf("Rotation: %i, Flip: %i\n", pr->transformation.rotation, pr->transformation.flip);
}

