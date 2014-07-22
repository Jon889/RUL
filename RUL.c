#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "RUL.h"
#include "List.h"
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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_PURPLE     "\x1b[35m"
#define ANSI_BOLD     "\x1b[1m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char * seekAfter(char **str, char chr) {
    return *str = strchr(*str, chr) + 1;
}
bool parseInstanceID(char *str, InstanceID *iid, int *rotationIdx);





Piece * list_pieceWithTextureID(List *list, InstanceID textureID) {
    Piece *piece = NULL;
    for (each_in_list(list, piece)) {
        if (piece->textureIID == textureID) {
            return piece;
        }
    }
    return NULL;
}


char * str_dup(char *str);

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
            latestPiece->network1Name = str_dup("MonoRail");
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


char * str_dup(char *str) {
    char *newString = calloc(strlen(str) + 1, sizeof(char));
    strcpy(newString, str);
    return newString;
}

int countChar(char *str, char chr) {
    int count = 0;
    while (*str != '\0') {
        if (*str++ == chr) {
            count++;
        }
    }
    return count;
}
#define READ_FLAGS(nf) nf.west = strtol(strtok(NULL, ","), NULL, 10);\
nf.north = strtol(strtok(NULL, ","), NULL, 10);\
nf.east = strtol(strtok(NULL, ","), NULL, 10);\
nf.south = strtol(strtok(NULL, ","), NULL, 10);
bool readPiece(char *str, List *list) {
    /*
     ;IID,simple,n1name,w1,n1,e1,s1,ow1,on1,oe1,os1,piece name(12)
     ;IID,simple,n1name,w1,n1,e1,s1,piece name(8)
     ;IID,simple,n1name,w1,n1,e1,s1,ow1,on1,oe1,os1,n2name,w2,n2,e2,s2,piece name(17)
     ;IID,simple,n1name,w1,n1,e1,s1,n2name,w2,n2,e2,s2,piece name(13)
     */
    int fieldCount = countChar(str, ',') + 1;
    int SingleOverrideFieldCount = 12;
    int SingleBaseFieldCount = 8;
    int DualOverrideFieldCount = 17;
    int DualBaseFieldCount = 13;
    if (fieldCount != SingleOverrideFieldCount && fieldCount != SingleBaseFieldCount
        && fieldCount != DualOverrideFieldCount && fieldCount != DualBaseFieldCount) {
        fprintf(stderr, "unexpected number of fields, has %i, expected %i, %i, %i or %i.\n", fieldCount,
               SingleOverrideFieldCount, SingleBaseFieldCount, DualOverrideFieldCount, DualBaseFieldCount);
        return false;
    }
    bool hasOverrideFlags = fieldCount == SingleOverrideFieldCount || fieldCount == DualOverrideFieldCount;
    bool isDualPiece = fieldCount == DualBaseFieldCount || fieldCount == DualOverrideFieldCount;
    InstanceID iid = strtol(strtok(str, ","), NULL, 16);
    bool simple = strcmp(strtok(NULL,  ","), "S") == 0;
    char *n1Name = strtok(NULL, ",");
    NetworkFlags n1;
    READ_FLAGS(n1);
    NetworkFlags o1;
    if (hasOverrideFlags) {
        READ_FLAGS(o1);
    }
    char *n2name;
    NetworkFlags n2;
    if (isDualPiece) {
        n2name = strtok(NULL, ",");
        READ_FLAGS(n2);
    }
    char *name = strtok(NULL, ",");
    while (isspace(name[strlen(name) - 1])) {
        name[strlen(name) - 1] = '\0';
    }
    Piece *newPiece = piece_create(iid, str_dup(name), simple);
    newPiece->network1Name = str_dup(n1Name);
    PieceRotation *baseRotation = pieceRotation_create_s(n1, 0, 0);
    if (hasOverrideFlags) {
        pieceRotation_addOverride1_s(baseRotation, o1);
    }
    if (isDualPiece) {
        int i = 0;
        char *n2subname = strtok(n2name, "|");
        while (n2subname != NULL) {
            newPiece->network2Names[i++] = str_dup(n2subname);
            n2subname = strtok(NULL, "|");
        }
        
        pieceRotation_addNetwork2_s(baseRotation, n2);
    }
    piece_fillRotationsFromBase(newPiece, baseRotation);
    list_addValue(list, newPiece);
    return true;
}
bool lineIsSpace(char *str) {
    while (*str != '\0') {
        if (!isspace(*str++)) {
            return false;
        }
    }
    return true;
}
bool readFalseIntersection(char *str, List *ignoreList) {
    InstanceID *iid = calloc(1, sizeof(InstanceID));
    if (parseInstanceID(str, iid, NULL)) {
        list_addValue(ignoreList, iid);
    }
    return true;
}

typedef enum {
    NoSection,
    OverridePieceSection,
    BasePieceSection,
    FalseIntersectionSection
} MapSection;
void readMap(char *path, List *pieceList, List *ignoreList) {
    FILE *fp = fopen(path , "r");
    if (fp == NULL) {
        fprintf(stderr, ANSI_BOLD ANSI_COLOR_RED "Error: unable to open file %s\n" ANSI_COLOR_RESET, path);
        exit(1);
    }
    char str[600];
    MapSection currentSection = NoSection;
    int line = 0;
    while (fgets(str, 600, fp)!= NULL) {
        line++;
        if (lineIsSpace(str)
            || str[0] == ';') {
        } else if (str[0] == '[') {
            char *opStr = "[OverridePieces]";
            char *bpStr = "[BasePieces]";
            char *fiStr = "[FalseIntersections]";
            if (strncmp(str, opStr, strlen(opStr)) == 0) {
                currentSection = OverridePieceSection;
            } else if (strncmp(str, bpStr, strlen(bpStr)) == 0) {
                currentSection = BasePieceSection;
            } else if (strncmp(str, fiStr, strlen(fiStr)) == 0) {
                currentSection = FalseIntersectionSection;
            } else {
                currentSection = NoSection;
                fprintf(stderr, ANSI_BOLD "%s:%i: " ANSI_COLOR_PURPLE "Warning:" ANSI_COLOR_RESET ANSI_BOLD " unknown section %s" ANSI_COLOR_RESET, path, line, str);
            }
        } else {
            int errorLen = fprintf(stderr, ANSI_BOLD "%s:%i: " ANSI_COLOR_RED "Error: " ANSI_COLOR_RESET ANSI_BOLD, path, line);
            switch (currentSection) {
                case FalseIntersectionSection:
                    readFalseIntersection(str, ignoreList);
                    break;
                case BasePieceSection:

                    break;
                case OverridePieceSection:
                    readPiece(str, pieceList);
                    break;
                case NoSection:
                    break;
            }
            fprintf(stderr, ANSI_COLOR_RESET "\r%*s\r", errorLen, "");
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

void readIntersections(char *path, char *networkName, List *list, List *ignoreList) {
    FILE *fp = fopen(path , "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    bool inRelevantSection = false;
    bool firstNetworkIsMonorail = false;
    char *otherNetworkName = NULL;
    char str[1200];
    while (fgets(str, 1200, fp)!= NULL) {
        if (str[0] == '[') {
            char *mrStr = strstr(str, "MonoRail");
            if (mrStr != NULL) {
                inRelevantSection = true;
                firstNetworkIsMonorail = (mrStr == str + 1);
                if (otherNetworkName != NULL) {
                    free(otherNetworkName);
                    otherNetworkName = NULL;
                }
                if (firstNetworkIsMonorail) {
                    otherNetworkName = str + 1 + strlen("MonoRail");
                    *strstr(str, "IntersectionSolutions") = '\0';
                } else {
                    otherNetworkName = str + 1;
                    *strstr(str, "MonoRail") = '\0';
                }
                otherNetworkName = str_dup(otherNetworkName);
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
                InstanceID *iid;
                bool ignore = false;
                for (each_in_list(ignoreList, iid)) {
                    if (*iid == textureIID) {
                        ignore = true;
                    }
                }
                if (!ignore) {
                    Rotation rot = strtol(seekAfter(&line, ','), NULL, 10);
                    Flip flip = strtol(seekAfter(&line, ','), NULL, 10);
                    if (rot == 0 && flip == 0) {
                        Piece *testPiece = piece_create(textureIID, NULL, false);
                        testPiece->network1Name = str_dup("MonoRail");
                        testPiece->network2Names[0] = str_dup(otherNetworkName);
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
    if (otherNetworkName != NULL) {
        free(otherNetworkName);
    }
    
}

void list_map(List *list, void (*fn)(void *)) {
    void *value = NULL;
    for (each_in_list(list, value)) {
        fn(value);
    }
}

void piece_print(Piece *piece) {
    printf("0x%08x %s %s\n", piece->textureIID, piece->network1Name, piece->network2Names[0]);
}

bool parseRotation(char *str, Rotation *rotation) {
    if (str == NULL) {
        printf("Missing rotation\n");
        return false;
    }
    Rotation rot = strtol(str, NULL, 10);
    if (rot < 0 || rot > 3) {
        printf("Invalid rotation, must be 0,1,2 or 3\n");
        return false;
    }
    *rotation = rot;
    return true;
}
bool parseFlip(char *str, Flip *flip) {
    if (str == NULL) {
        printf("Missing flip\n");
        return false;
    }
    Flip f = strtol(str, NULL, 10);
    if (f != 0 && f != 1) {
        printf("Invalid flip, must be 0 or 1\n");
        return false;
    }
    *flip = f;
    return true;
}




//Piece *list_findPieceWithNetworkFlags(List *list, char *n1name, NetworkFlags *n1flags, char *n2names[10], NetworkFlags *n2flags, Rotation *rot);
int attemptRULsForPieces(List *list, Piece *leftPiece, Piece *rightPiece) {
    int count = 0;
    for (int left = 0; left < (leftPiece->simple ? 4 : 8); left++) {
        for (int right = 0; right < (rightPiece->simple ? 4 : 8); right++) {
            PieceRotation *rightRotation = rightPiece->rotations[right];
            if (pieceRotation_fitsLeftOfPieceRotation(leftPiece->rotations[left], rightRotation)) {
                Rotation rot;
                NetworkFlags desiredFlags = *rightRotation->override1;
                desiredFlags.west = leftPiece->rotations[left]->override1->east;
                Piece *rightOverridePiece = list_findPieceWithNetworkFlags(list, "HSR", &desiredFlags, rightPiece->network2Names, rightRotation->network2, &rot);
                if (rightOverridePiece == NULL) {
                    desiredFlags.west = leftPiece->rotations[left]->network1->east;
                    rightOverridePiece = list_findPieceWithNetworkFlags(list, "HSR", &desiredFlags, rightPiece->network2Names, rightRotation->network2, &rot);
                }
                if (rightOverridePiece == NULL) {
//                    printf(";missing piece 0x%08x | 0x%08x -> ", leftPiece->textureIID, rightPiece->textureIID);
//                    networkFlags_print(rightRotation->network1);
//                    printf(" n2: ");
//                    networkFlags_print(rightRotation->network2);
//                    printf("\n");
                } else {
                    if (piece_printRUL(leftPiece, left, rightPiece, right, leftPiece, left, rightOverridePiece, rot)) {
                        printf("\n");
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

bool parseInstanceID(char *str, InstanceID *iid, int *rotationIdx) {
    char *iidStr = strtok(str, ",");
    if (iidStr == NULL) {
        printf("Invalid IID string, must be in format 0x######## or 0x########,r,f\n");
        return false;
    }
    *iid = strtol(iidStr, NULL, 16);
    Rotation rotation;
    Flip flip;
    char *rStr = strtok(NULL, ",");
    if (rStr == NULL) {
        return true;
    }
    if (!parseRotation(rStr, &rotation)) {
        return false;
    }
    if (!parseFlip(strtok(NULL, ","), &flip)) {
        return false;
    }
    *rotationIdx = rotation + flip * 4;
    return true;
}

int main(int argc, char **argv) {
    assert(argc >= 2);
    List *list = list_create();
    List *ignoreList = list_create();
    readRUL("/Users/jonathan/Desktop/file_dec00000000.rul", list);
    readMap(argv[1], list, ignoreList);
    readIntersections("/Users/jonathan/Desktop/file_dec00000002.rul", NULL, list, ignoreList);

    

    if (argc >= 3) {
        int commandIdx = 2;
        if (strcmp(argv[commandIdx], "print") == 0) {
            if (argc == 4) {//RUL map.txt print IID[,r,f]
                int rot = -1;
                InstanceID iid;
                if (!parseInstanceID(argv[commandIdx + 1], &iid, &rot)) {
                    return 1;
                }
                Piece *piece = list_pieceWithTextureID(list, iid);
                if (piece == NULL) {
                    printf("No piece with IID: 0x%08x\n", iid);
                    return 1;
                }
                if (rot == -1) {
                    for (int i = 0; i < 8; i++) {
                        piece_printAsciiArt(piece, i);
                        printf("\n");
                    }
                } else {
                    piece_printAsciiArt(piece, rot);
                }
                return 0;
            } else if (argc == 5) {//RUL map.txt print IID[,r,f] IID[r,f]
                int leftRot = -1;
                InstanceID leftIID;
                if (!parseInstanceID(argv[commandIdx + 1], &leftIID, &leftRot)) {
                    return 1;
                }
                int rightRot = -1;
                InstanceID rightIID;
                if (!parseInstanceID(argv[commandIdx + 2], &rightIID, &rightRot)) {
                    return 1;
                }
                Piece *leftPiece = list_pieceWithTextureID(list, leftIID);
                if (leftPiece == NULL) {
                    printf("No piece with IID: 0x%08x\n", leftIID);
                    return 1;
                }
                Piece *rightPiece = list_pieceWithTextureID(list, rightIID);
                if (rightPiece == NULL) {
                    printf("No piece with IID: 0x%08x\n", rightIID);
                    return 1;
                }
                for (int left = (leftRot == -1 ? 0 : leftRot); left <= (leftRot == -1 ? (leftPiece->simple ? 3 : 7) : leftRot); left++) {
                    for (int right = (rightRot == -1 ? 0 : rightRot); right <= (rightRot == -1 ? (rightPiece->simple ? 3 : 7) : rightRot); right++) {
                        piece_printAsciiArtPair(leftPiece, left, rightPiece, right);
                    }
                }
                return 0;
            } else {
                printf("Incorrect parameters\n");
                printf("To print the rotation(s) of a piece:\n\t%s %s print 0x########[,r,f]\n", argv[0], argv[1]);
                printf("To print the possible pairings of 2 pieces:\n\t%s %s print 0x########[,r,f] 0x########[,r,f]\n", argv[0], argv[1]);
                return 1;
            }
        } else if (strcmp(argv[commandIdx], "find") == 0) {
            //find n1name W N E S [n2name W N E S]
            if (argc >= 8) {
                char *n1name = argv[commandIdx + 1];
                NetworkFlags n1;
                n1.west = strtol(argv[commandIdx + 2], NULL, 10);
                n1.north = strtol(argv[commandIdx + 3], NULL, 10);
                n1.east = strtol(argv[commandIdx + 4], NULL, 10);
                n1.south = strtol(argv[commandIdx + 5], NULL, 10);
                NetworkFlags n2;
                char *n2names[10];
                if (argc == 13) {
                    n2names[0] = argv[commandIdx + 6];
                    n2.west = strtol(argv[commandIdx + 7], NULL, 10);
                    n2.north = strtol(argv[commandIdx + 8], NULL, 10);
                    n2.east = strtol(argv[commandIdx + 9], NULL, 10);
                    n2.south = strtol(argv[commandIdx + 10], NULL, 10);
                }
            
                Rotation rot;
                Piece *piece = list_findPieceWithNetworkFlags(list, n1name, &n1, n2names, argc == 13 ? &n2 : NULL, &rot);
                if (piece == NULL) {
                    printf("No piece found\n");
                    return 0;
                }
                piece_printAsciiArt(piece, rot);
                return 0;
            }
        } else if (strcmp(argv[commandIdx], "list") == 0) {
            list_map(list, (void (*)(void *))&piece_print);
            return 0;
        } else if (strcmp(argv[commandIdx], "ruls") == 0) {
            if (argc == 3) {
                int totalRulsPrinted = 0;
                Piece *leftPiece = NULL;
                for (each_in_list(list, leftPiece)) {
                    if (strcmp(leftPiece->network1Name, "HSR") == 0) {
                        Piece *rightPiece = NULL;
                        for (each_in_list(list, rightPiece)) {
                            totalRulsPrinted += attemptRULsForPieces(list, leftPiece, rightPiece);
                        }
                    }
                }
                fprintf(stderr, ";%i RULs generated\n", totalRulsPrinted);
            } else if (argc == 4 || argc == 5) {
                InstanceID leftIID;
                if (parseInstanceID(argv[commandIdx + 1], &leftIID, NULL)) {
                    Piece *leftPiece = list_pieceWithTextureID(list, leftIID);
                    if (strcmp(leftPiece->network1Name, "HSR") != 0) {
                        printf("Left piece must be HSR\n");
                        return 1;
                    }
                    if (argc == 5) {
                        InstanceID rightIID;
                        if (parseInstanceID(argv[commandIdx + 2], &rightIID, NULL)) {
                            Piece *rightPiece = list_pieceWithTextureID(list, rightIID);
                            attemptRULsForPieces(list, leftPiece, rightPiece);
                        }
                    } else {
                        Piece *rightPiece = NULL;
                        for (each_in_list(list, rightPiece)) {
                            attemptRULsForPieces(list, leftPiece, rightPiece);
                        }
                    }
                    return 0;
                }
                
            }
        }
    }

    list_destroy(list);
}
