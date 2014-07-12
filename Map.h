#ifndef MAP_H
#define MAP_H
#include "Piece.h"
#include "List.h"
#include "typedefs.h"

struct MappingValue_s {
    Piece *overridePiece;
    Piece *basePiece;
};

struct Map_s {
    char *overrideName;
    char *baseName;
    List *mapping;
};

Map *map_createEmpty(void);
void map_destroy(Map *map);


MappingValue * mappingValue_createEmpty(void);
void mappingValue_print(MappingValue *mv);
#endif
