#include "NetworkFlags.h"
#include <stdlib.h>
#include <stdio.h>

NetworkFlags::NetworkFlags(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south) {
    this->west = west;
    this->north = north;
    this->east = east;
    this->south = south;
}
NetworkFlags::NetworkFlags() : NetworkFlags(0,0,0,0) {}

NetworkFlags::NetworkFlags(NetworkFlags *flags) : NetworkFlags(flags->west, flags->north, flags->east, flags->south) {}

void NetworkFlags::copy(NetworkFlags *dest) {
    dest->west = west;
    dest->north = north;
    dest->east = east;
    dest->south = south;
}
NetworkFlags::~NetworkFlags() {}
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

void NetworkFlags::mirror() {
    NetworkFlag oldWest = west;
    west = east;
    east = oldWest;
    networkFlag_mirror(&west);
    networkFlag_mirror(&north);
    networkFlag_mirror(&east);
    networkFlag_mirror(&south);
}

void NetworkFlags::rotate90MC() {
    NetworkFlag newWest = north;
    south = west;
    east = south;
    north = east;
    west = newWest;
}
void NetworkFlags::rotate90C() {
    NetworkFlag oldSouth = south;
    south = east;
    east = north;
    north = west;
    west = oldSouth;
}
bool NetworkFlags::equal(NetworkFlags *nf2) {
    return (west == nf2->west && north == nf2->north
            && east == nf2->east && south == nf2->south);
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

void NetworkFlags::print() {
    printf("W:%i N:%i E:%i S:%i", west, north, east, south);
}
