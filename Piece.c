#include "Piece.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "PieceRotation.h"
#include "NetworkFlags.h"

bool piece_hasCommonNetwork2(char *p1network2Names[10], char *p2network2Names[10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (strcmp(p1network2Names[i], p2network2Names[j]) == 0) {
                return true;
            }
        }
    }
    return false;
}
void piece_printAsciiArt(Piece *p, int rotation) {
    NetworkFlags *piece = p->rotations[rotation]->network1;
    NetworkFlags *override = p->rotations[rotation]->override1;
    NetworkFlags *n2 = p->rotations[rotation]->network2;
    printf("  +-------+  +-------+   +-------+\n");
    printf("  |  %3d  |  |  %3d  |   |  %3d  |\n", piece->north, override->north, n2->north);
    printf("M:|%-3d %3d|O:|%-3d %3d|N2:|%-3d %3d|\t0x%08x,%i,%i %s\n", piece->west, piece->east, override->west, override->east, n2->west, n2->east, p->textureIID, p->rotations[rotation]->transformation.rotation, p->rotations[rotation]->transformation.flip, p->name);
    printf("  |  %3d  |  |  %3d  |   |  %3d  |\n", piece->south, override->south, n2->south);
    printf("  +-------+  +-------+   +-------+\n");
}
void printTopBottomOfPair(NetworkFlag f1, NetworkFlag f2) {
    printf("   |  %2d%s  |  %2d%s  |", f1, f1 < 100 ? " " : "", f2, f2 < 100 ? " " : "");
}
void piece_printAsciiArtPair(Piece *piece, int rotation1, Piece *p2, int rotation2) {
    NetworkFlags *nf1 = piece->rotations[rotation1]->network1;
    NetworkFlags *nf2 = p2->rotations[rotation2]->network1;
    NetworkFlags *nf12 = piece->rotations[rotation1]->network2;
    NetworkFlags *nf22 = p2->rotations[rotation2]->network2;
    NetworkFlags *v1 = piece->rotations[rotation1]->override1;
    NetworkFlags *v2 = p2->rotations[rotation2]->override1;
    Flip f1 = rotation1 >= 4 ? 1 : 0;
    Rotation r1 =  rotation1 - ((f1 == 1) ? 4 : 0);
    Flip f2 = rotation2 >= 4 ? 1 : 0;
    Rotation r2 =  rotation2 - ((f2 == 1) ? 4 : 0);
    bool bothNoN2 = piece->network2Names[0] != NULL && p2->network2Names[0] != NULL;
    printf("\n");
    printf("      %i,%i     %i,%i\n", r1, f1, r2, f2);
#define edgeFormat "   +-------+-------+"
    printf(edgeFormat);
    if (bothNoN2) {
        printf(edgeFormat);
    }
    printf(edgeFormat);
    printf("\n");
    
    printTopBottomOfPair(nf1->north, nf2->north);
    if (bothNoN2) {
        printTopBottomOfPair(nf1->north, nf22->north);
    }
    printTopBottomOfPair(v1->north, v2->north);
    printf("\n");
    
#define middleFormat "%s:|%-3d %3d|%-3d %3d|"
    printf(middleFormat, "N1", nf1->west, nf1->east, nf2->west, nf2->east);
    if (bothNoN2) {
        printf(middleFormat, "N2", nf12->west, nf12->east, nf22->west, nf22->east);
    }
    printf(middleFormat, " O", v1->west, v1->east, v2->west, v2->east);
    printf("\n");
    

    printTopBottomOfPair(nf1->south, nf2->south);
    if (bothNoN2) {
        printTopBottomOfPair(nf1->south, nf22->south);
    }
    printTopBottomOfPair(v1->south, v2->south);
    printf("\n");
    
    printf(edgeFormat);
    if (bothNoN2) {
        printf(edgeFormat);
    }
    printf(edgeFormat);
    printf("\n");
    printf("    %-7s %-7s", piece->network1Name, p2->network1Name);
    if (bothNoN2) {
        printf("   %-7s %-7s", piece->network2Names[0], p2->network2Names[0]);
    }
    printf("\n");
}


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

Piece *list_findPieceWithNetworkFlags(List *list, char *n1name, NetworkFlags *n1flags, char *n2names[10], NetworkFlags *n2flags, Rotation *rot) {
    Piece *piece = NULL;
    for (each_in_list(list, piece)) {
        if (strcmp(piece->network1Name, n1name) == 0) {
            for (int i = 0; i < (piece->simple ? 4 : 8); i++) {
                if (networkFlags_equal(n1flags, piece->rotations[i]->network1)) {
                    if ((n2names[0] == NULL && piece->network2Names[0] == NULL)) {
                        *rot = i;
                        return piece;
                    } else if ((n2names[0] != NULL && piece->network2Names[0] != NULL)) {
                        if ((networkFlags_equal(n2flags, piece->rotations[i]->network2)
                             && strcmp(n2names[0], piece->network2Names[0]) == 0)) {
                            *rot = i;
                            return piece;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

Piece * piece_createEmpty(void) {
    return piece_create(0, NULL, false);
}
Piece * piece_create(InstanceID tid, char *name, bool simple) {
    Piece *p = calloc(1, sizeof(Piece));
    p->textureIID = tid;
    p->name = name;
    p->simple = simple;
    p->network1Name = NULL;
    return p;
}
Piece * piece_createPrevent(void) {
    Piece *np = piece_createEmpty();
    for (int i = 0; i < 8; i++) {
        np->rotations[i] = pieceRotation_create(0,0,0,0,0,0);
    }
    return np;
}

void piece_destroy(Piece *piece) {
    for (int i = 0; i < 8; i++) {
        pieceRotation_destroy(piece->rotations[i]);
    }
    free(piece);
}



Piece *piece_clone(Piece *piece) {
    Piece *np = piece_create(piece->textureIID,
                             piece->name,
                             piece->simple);
    for (int i = 0; i < 8; i++) {
        np->rotations[i] = pieceRotation_clone(piece->rotations[i]);
    }
    return np;
}

Rotation rotation_next90C(Rotation *rot) {
    if (*rot == Rotation_270) {
        *rot = Rotation_0;
    } else {
        *rot += 1;
    }
    return *rot;
}

void piece_fillRotationsFromBase(Piece *piece, PieceRotation *baseRotation) {
    Rotation rot = baseRotation->transformation.rotation;
    PieceRotation *t;
    int ma = baseRotation->transformation.flip == 1 ? 4 : 0; //mirror adjustment;
    piece->rotations[rot + ma] = pieceRotation_clone(baseRotation);
    
    t = pieceRotation_rotate90C(pieceRotation_clone(piece->rotations[rot + ma]));
    piece->rotations[rotation_next90C(&rot) + ma] = t;

    t = pieceRotation_rotate90C(pieceRotation_clone(piece->rotations[rot + ma]));
    piece->rotations[rotation_next90C(&rot) + ma] = t;
    
    t = pieceRotation_rotate90C(pieceRotation_clone(piece->rotations[rot + ma]));
    piece->rotations[rotation_next90C(&rot) + ma] = t;
    
    int ma2 = baseRotation->transformation.flip == 1 ? 0 : 4; //anti mirror adjustment;
    piece->rotations[0 + ma2] = pieceRotation_mirror(pieceRotation_clone(piece->rotations[0 + ma]));
    piece->rotations[1 + ma2] = pieceRotation_mirror(pieceRotation_clone(piece->rotations[1 + ma]));
    piece->rotations[2 + ma2] = pieceRotation_mirror(pieceRotation_clone(piece->rotations[2 + ma]));
    piece->rotations[3 + ma2] = pieceRotation_mirror(pieceRotation_clone(piece->rotations[3 + ma]));
    
}

bool piece_printRUL(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri) {
    if (ll->textureIID == rl->textureIID && lr->textureIID == rr->textureIID) {
        return false;
    }
    printf("0x%08x,%i,%i,0x%08x,%i,%i",
           ll->textureIID, ll->rotations[lli]->transformation.rotation, ll->rotations[lli]->transformation.flip,
           lr->textureIID, lr->rotations[lri]->transformation.rotation, lr->rotations[lri]->transformation.flip);
    if (rr != NULL) {
        printf("=0x%08x,%i,%i,0x%08x,%i,%i",
               rl->textureIID, rl->rotations[rli]->transformation.rotation, rl->rotations[rli]->transformation.flip,
               rr->textureIID, rr->rotations[rri]->transformation.rotation, rr->rotations[rri]->transformation.flip);
    }
    return true;
}
//returns number of ruls printed
int piece_printRulesOfPieces(Piece *leftPiece, Piece *rightPiece, Piece *rightOverridePiece, bool asciiArt, bool printFailures) {
    int retval = 0;
    for (int left = 0; left < (leftPiece->simple ? 4 : 8); left++) {
        for (int right = 0; right < (rightPiece->simple ? 4 : 8); right++) {
            if (printFailures) {
                if (pieceRotation_fitsLeftOfPieceRotation(leftPiece->rotations[left], rightPiece->rotations[right])) {
                    printf(ANSI_COLOR_GREEN);
                } else {
                    printf(ANSI_COLOR_RED);
                }
            }
            if (printFailures || pieceRotation_fitsLeftOfPieceRotation(leftPiece->rotations[left], rightPiece->rotations[right])) {
                retval++;
                if (asciiArt) {
                    piece_printAsciiArtPair(leftPiece, left, rightPiece, right);
                }
                printf("\n    ");
                piece_printRUL(leftPiece, left, rightPiece, right, leftPiece, left, rightOverridePiece, right);
            }
            if (printFailures) {
                printf(ANSI_COLOR_RESET);
            }
        }
    }
    return retval;
}



