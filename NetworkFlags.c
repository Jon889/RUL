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
        case 52:
            *flag = 32;
            break;
        case 32:
            *flag = 52;
            break;
        case 23:
            *flag = 21;
            break;
        case 21:
            *flag = 23;
            break;
        case 42:
            *flag = 22;
            break;
        case 22:
            *flag = 42;
            break;
    }
    if (*flag > 100) {
        if (*flag % 10 == 3) {
            *flag -= 2;
        } else if (*flag % 10 == 1) {
            *flag += 2;
        }
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
bool networkFlags_equal(NetworkFlags *nf1, NetworkFlags *nf2) {
    return (nf1->west == nf2->west && nf1->north == nf2->north
            && nf1->east == nf2->east && nf1->south == nf2->south);
}
bool networkFlag_matches(NetworkFlag left, NetworkFlag right, bool zerosCanMatch) {
    if (left == right && (zerosCanMatch || left != NetworkFlag_None)) {
        return true;
    }
    if ((left == 11 && right == 1)
        || (left == 1 && right == 11)) {
        return true;
    }
    if ((left == 13 && right == 3)
        || (left == 3 && right == 13)) {
        return true;
    }
    return false;
}

void networkFlags_print(NetworkFlags *nf) {
    printf("W:%i N:%i E:%i S:%i", nf->west, nf->north, nf->east, nf->south);
}
