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
NetworkFlags *networkFlags_clone(NetworkFlags *flags) {
    return networkFlags_create(flags->west, flags->north, flags->east, flags->south);
}
void networkFlags_copy(NetworkFlags *dest, NetworkFlags *src) {
    dest->west = src->west;
    dest->north = src->north;
    dest->east = src->east;
    dest->south = src->south;
}
void networkFlags_destroy(NetworkFlags *nf) {
    free(nf);
}
void networkFlag_mirror(NetworkFlag *flag) {
    
    switch (*flag) {
        case 1:
        case 11:
        case 21:
            *flag += 2;
            break;
        case 3:
        case 13:
        case 23:
            *flag -= 2;
            break;
        case 22:
        case 32:
            *flag += 20;
            break;
        case 42:
        case 52:
            *flag -= 20;
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
    networkFlag_mirror(&piece->west);
    networkFlag_mirror(&piece->north);
    networkFlag_mirror(&piece->east);
    networkFlag_mirror(&piece->south);
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
