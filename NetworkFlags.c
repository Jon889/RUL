#include "NetworkFlags.h"
#include <stdlib.h>
#include <stdio.h>

NetworkFlags * networkFlags_create(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    NetworkFlags *flags = calloc(1, sizeof(NetworkFlags));
    flags->west = west;
    flags->north = north;
    flags->east = east;
    flags->south = south;
    return flags;
}
void networkFlags_destroy(NetworkFlags *nf) {
    free(nf);
}
void mirrorLRFlipper(NetworkFlag *flag) {
    switch (*flag) {
        case 1:
            *flag = 3;
            break;
        case 3:
            *flag = 1;
            break;
        case 11:
            *flag = 13;
            break;
        case 13:
            *flag = 11;
            break;
    }
}

void networkFlags_mirror(NetworkFlags *piece) {
    NetworkFlag oldWest = piece->west;
    piece->west = piece->east;
    piece->east = oldWest;
    mirrorLRFlipper(&piece->west);
    mirrorLRFlipper(&piece->north);
    mirrorLRFlipper(&piece->east);
    mirrorLRFlipper(&piece->south);
}

void networkFlags_rotate90MC(NetworkFlags *piece) {
    NetworkFlag newWest = piece->north;
    piece->south = piece->west;
    piece->east = piece->south;
    piece->north = piece->east;
    piece->west = newWest;
}
void networkFlags_rotate90C(NetworkFlags *piece) {
    NetworkFlag oldSouth = piece->south;
    piece->south = piece->east;
    piece->east = piece->north;
    piece->north = piece->west;
    piece->west = oldSouth;
}

bool networkFlags_fitsLeftOfNetworkFlags(NetworkFlags *left, NetworkFlags *right, bool zerosCanMatch) {
    if (left->east == right->west && (zerosCanMatch || left->east != NetworkFlag_None)) {
        return true;
    }
    if ((left->east == 11 && right->west == 1)
        || (left->east == 1 && right->west == 11)) {
        return true;
    }
    if ((left->east == 13 && right->west == 3)
        || (left->east == 3 && right->west == 13)) {
        return true;
    }
    return false;
}

void networkFlags_print(NetworkFlags *nf) {
    printf("W:%i N:%i E:%i S:%i\n", nf->west, nf->north, nf->east, nf->south);
}
