#ifndef NETWORKFLAGS_H
#define NETWORKFLAGS_H
#include "RUL.h"
#include <stdbool.h>
struct NetworkFlags_s {
    NetworkFlag west;
    NetworkFlag north;
    NetworkFlag east;
    NetworkFlag south;
};

NetworkFlags * networkFlags_create(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south);
void networkFlags_destroy(NetworkFlags *nf);
void networkFlags_mirror(NetworkFlags *piece);
void networkFlags_rotate90MC(NetworkFlags *piece);
void networkFlags_rotate90C(NetworkFlags *piece);
bool networkFlags_fitsLeftOfNetworkFlags(NetworkFlags *left, NetworkFlags *right, bool zerosCanMatch);
void networkFlags_print(NetworkFlags *nf);
#endif
