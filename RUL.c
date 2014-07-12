#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "RUL.h"
#include "List.h"
#include "Map.h"
#include "Piece.h"
#include "PieceRotation.h"
#include "NetworkFlags.h"

const NetworkFlag NetworkFlag_None          = 0;
const NetworkFlag NetworkFlag_Left_45       = 1;
const NetworkFlag NetworkFlag_Straight      = 2;
const NetworkFlag NetworkFlag_Right_45      = 3;
const NetworkFlag NetworkFlag_Shared_Median = 4;
const NetworkFlag NetworkFlag_Left_Blend    = 11;
const NetworkFlag NetworkFlag_Right_Blend   = 13;
const Rotation Rotation_0   = 0;
const Rotation Rotation_90  = 1;
const Rotation Rotation_180 = 2;
const Rotation Rotation_270 = 3;
const Flip Flip_None     = 0;
const Flip Flip_Mirrored = 1;



char * seekAfter(char **str, char chr) {
    return *str = strchr(*str, chr) + 1;
}






Piece * list_pieceWithTextureID(List *list, InstanceID textureID) {
    Piece *piece = NULL;
    for (each_in_list(list, piece)) {
        if (piece->textureIID == textureID) {
            return piece;
        }
    }
    return NULL;
}




void readRUL(char *path, List *list) {
    FILE *fp = fopen(path , "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    char str[60];
    Piece *latestPiece = NULL;
    NetworkFlags holder;
    while (fgets(str, 60, fp)!= NULL) {
    loopstart: {}
        char *istr = str;
        if (str[0] == '1') {
            assert(latestPiece == NULL);
            latestPiece = piece_createEmpty();
            holder.west = atoi(seekAfter(&istr, ','));
            holder.north = atoi(seekAfter(&istr, ','));
            holder.east = atoi(seekAfter(&istr, ','));
            holder.south = atoi(seekAfter(&istr, ','));
        }
        if (str[0] == '2') {
            latestPiece = NULL;
            while (fgets(str, 60, fp)!= NULL) {
                if (str[0] == '1') {
                    goto loopstart;
                }
            }
        }
        if (str[0] == '3') {
            assert(latestPiece != NULL);
            assert(*seekAfter(&istr, ',') == '0'); //refences center piece (don't support multiple piece RULs)
            latestPiece->textureIID = strtol(seekAfter(&istr, ','), NULL, 16);
            PieceRotation *rot = pieceRotation_create(holder.west, holder.north, holder.east, holder.south, atoi(seekAfter(&istr, ',')), atoi(seekAfter(&istr, ',')));
            piece_fillRotationsFromBase(latestPiece, rot);
            list_addValue(list, latestPiece);
            latestPiece = NULL;
        }
    }
    fclose(fp);
}





void readVirtualPieces(FILE *fp, List *pieceList) {
    char str[60];
    while (fgets(str, 60, fp)!= NULL) {
        if (strncmp(str, "#End", 4) == 0) {
            return;
        }
        char *line = str;
        InstanceID newPieceID = strtol(line, NULL, 16);
        InstanceID basePieceID = strtol(seekAfter(&line, ','), NULL, 16);
        Piece *basePiece = list_pieceWithTextureID(pieceList, basePieceID);
        Piece *newPiece = piece_clone(basePiece);
        newPiece->textureIID = newPieceID;
        list_addValue(pieceList, newPiece);
    }
}

void readMap(char *path, Map *map, List *pieceList) {
    FILE *fp = fopen(path , "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    char str[60];
    while (fgets(str, 60, fp)!= NULL) {
        if (str[0] == '#') {
            char *vpStr = "#VirtualPieces";
            if (strncmp(str, vpStr, strlen(vpStr)) == 0) {
                readVirtualPieces(fp, pieceList);
            } else {
                char *comma = strchr(str, ',');
                *comma = '\0';
                char *nl = strchr(comma + 1, '\n');
                if (nl != NULL) {
                    *nl = '\0';
                }
                map->baseName = calloc(60, sizeof(char));
                map->overrideName = calloc(60, sizeof(char));
                strncpy(map->baseName, str + 1, 60);
                strncpy(map->overrideName, comma + 1, 60);
            }
        } else {
            MappingValue *value = mappingValue_createEmpty();
            char *line = str;
            int baseIID = strtol(line, NULL, 16);
            value->basePiece = list_pieceWithTextureID(pieceList, baseIID);
            if (value->basePiece != NULL) {
            
                int overtexid = strtol(seekAfter(&line, ','), NULL, 16);
                char *simple = seekAfter(&line, ',');
                if (simple[0] == 'S') {
                    value->basePiece->simple = true;
                }
                value->overridePiece = piece_clone(value->basePiece);
                value->overridePiece->textureIID = overtexid;
                
                char *name = seekAfter(&line, ',');
                char *nl = strchr(name, '\n');
                if (nl != NULL) {
                    *nl = '\0';
                }
                value->basePiece->name = calloc(strlen(name) + 1, sizeof(char));
                strcpy(value->basePiece->name, name);
                list_addValue(map->mapping, value);
            }
        }
    }
}

NetworkFlag intersectionFlags(char **str) {
    char flagStr[3];
    flagStr[0] = *(*str)++;
    flagStr[1] = *(*str)++;
    flagStr[2] = '\0';
    return strtol(flagStr, NULL, 10);
}

void readIntersections(char *path, char *networkName, List *list) {
    FILE *fp = fopen(path , "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    bool inRelevantSection = false;
    bool firstNetworkIsMonorail = false;
    char str[1200];
    while (fgets(str, 1200, fp)!= NULL) {
        if (str[0] == '[') {
            char *mrStr = strstr(str, "MonoRail");
            if (mrStr != NULL) {
                inRelevantSection = true;
                firstNetworkIsMonorail = (mrStr == str + 1);
            } else {
                inRelevantSection = false;
            }
        } else if (inRelevantSection && str[0] != ';') {
//            rot0=0x02000200,0x00020002,0x03010100,0,0

            char *line = str;
            if (strchr(line, '=') != NULL) {
                seekAfter(&line, '=');
                line += 2;
                NetworkFlags n1;
                n1.south = intersectionFlags(&line);
                n1.east = intersectionFlags(&line);
                n1.north = intersectionFlags(&line);
                n1.west = intersectionFlags(&line);
                seekAfter(&line, ',');
                line += 2;
                NetworkFlags n2;
                n2.south = intersectionFlags(&line);
                n2.east = intersectionFlags(&line);
                n2.north = intersectionFlags(&line);
                n2.west = intersectionFlags(&line);
                InstanceID textureIID = strtol(seekAfter(&line, ','), NULL, 16);
                Rotation rot = strtol(seekAfter(&line, ','), NULL, 10);
                Flip flip = strtol(seekAfter(&line, ','), NULL, 10);
                if (rot == 0 && flip == 0) {
                    Piece *testPiece = piece_create(textureIID, NULL, false);
                    NetworkFlags monorail = n2;
                    NetworkFlags other = n1;
                    if (firstNetworkIsMonorail) {
                        monorail = n1;
                        other = n2;
                    }
                    PieceRotation *rot0 = pieceRotation_create(monorail.west,monorail.north,monorail.east,monorail.south,rot,flip);
                    pieceRotation_addNetwork2(rot0, other.west,other.north,other.east,other.south);
                    piece_fillRotationsFromBase(testPiece, rot0);
                    
                    list_addValue(list, testPiece);
                }
            }
        }
    }

    
}

void list_map(List *list, void (*fn)(void *)) {
    void *value = NULL;
    for (each_in_list(list, value)) {
        fn(value);
    }
}

void piece_print(Piece *piece) {
    printf("0x%08x %s\n", piece->textureIID, piece->name);
    for (int i = 0; i < 8; i++) {
        pieceRotation_print(piece->rotations[i]);
    }
}

int main(int argc, char **argv) {
    List *list = list_create();
    readRUL("/Users/jonathan/Desktop/file_dec00000000.rul", list);
    readIntersections("/Users/jonathan/Desktop/file_dec00000002.rul", NULL, list);
    Map *map = map_createEmpty();
    readMap("/Users/jonathan/RUL/hsr.txt", map, list);
    

    if (argc == 2) {
        int id = strtol(argv[1], NULL, 16);
        piece_printAsciiArt(list_pieceWithTextureID(list, id), 0);
        return 0;
    }
    
    if (argc == 3) {
        int leftId = strtol(argv[1], NULL, 16);
        int rightId = strtol(argv[2], NULL, 16);
        piece_printRulesOfPieces(list_pieceWithTextureID(list, leftId), list_pieceWithTextureID(list, rightId), NULL, true, true);
        return 0;
    }
    if (argc == 5) {
        int leftId = strtol(argv[1], NULL, 16);
        int leftRot = strtol(argv[2], NULL, 10);
        int rightId = strtol(argv[3], NULL, 16);
        int rightRot = strtol(argv[4], NULL, 10);
        piece_printAsciiArtPair(list_pieceWithTextureID(list, leftId), leftRot, list_pieceWithTextureID(list, rightId), rightRot);
        return 0;
    }
    MappingValue *mapValue = NULL;
    int totalRulsPrinted = 0;
    for (each_in_list(map->mapping, mapValue)) {
        printf(";%s %s next to...\n", map->overrideName, mapValue->basePiece->name);
        totalRulsPrinted += piece_printRules(mapValue->overridePiece, map, list);
        printf("\n");
    }
    printf(";%i RULs generated\n", totalRulsPrinted);
    list_destroy(list);
}
