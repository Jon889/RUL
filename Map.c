#include "Map.h"
#include <stdlib.h>
#include <stdio.h>

Map *map_createEmpty(void) {
    Map *map = calloc(1, sizeof(Map));
    map->mapping = list_create();
    return map;
}
void map_destroy(Map *map) {
    list_destroy(map->mapping);
    free(map);
}


MappingValue * mappingValue_createEmpty(void) {
    MappingValue *value = calloc(1, sizeof(MappingValue));
    return value;
}

void mappingValue_print(MappingValue *mv) {
    printf("0x%08x", mv->overridePiece->textureIID);
    printf(" -> ");
    printf("0x%08x", mv->basePiece->textureIID);
    printf("\n");
}
