#ifndef NETWORKFLAGS_H
#define NETWORKFLAGS_H
#include "RUL.h"
#include <stdbool.h>
class NetworkFlags {
    
public:
    NetworkFlag west;
    NetworkFlag north;
    NetworkFlag east;
    NetworkFlag south;

    NetworkFlags();
    
    NetworkFlags(NetworkFlag west, NetworkFlag north, NetworkFlag east, NetworkFlag south);
    ~NetworkFlags();
    void mirror();
    void copy(NetworkFlags *dest);
    NetworkFlags(NetworkFlags *flags);
    void rotate90MC();
    void rotate90C();
    bool matches(NetworkFlag right, bool zerosCanMatch);
    void print();
    bool equal(NetworkFlags *nf2);
};

bool networkFlag_matches(NetworkFlag left, NetworkFlag right, bool zerosCanMatch);
#endif
