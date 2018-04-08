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
void Piece::printAsciiArt(int rotation) {
    Piece *p = this;
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
void Piece::printAsciiArtPair(int rotation1, Piece *p2, int rotation2) {
    Piece *piece = this;
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
bool pieceRotation_matches(PieceRotation *pr, NetworkFlags *flags, NetworkFlags *n2flags, int *overrideMatchCount) {
    *overrideMatchCount = 0;
    //each flag must match either the override or base flag
    if (networkFlag_matches(pr->override1->west, flags->west, true)) {
        (*overrideMatchCount)++;
    } else if (!networkFlag_matches(pr->network1->west, flags->west, true)) {
        return false;
    }
    if (networkFlag_matches(pr->override1->north, flags->north, true)) {
        (*overrideMatchCount)++;
    } else if (!networkFlag_matches(pr->network1->north, flags->north, true)) {
        return false;
    }
    if (networkFlag_matches(pr->override1->east, flags->east, true)) {
        (*overrideMatchCount)++;
    } else if (!networkFlag_matches(pr->network1->east, flags->east, true)) {
        return false;
    }
    if (networkFlag_matches(pr->override1->south, flags->south, true)) {
        (*overrideMatchCount)++;
    } else if (!networkFlag_matches(pr->network1->south, flags->south, true)) {
        return false;
    }
    //network1/override1 is a match
    return pr->network2->equal(n2flags);
    
}
bool network2NamesCompatible(char *names1[10], char *names2[10]) {
    if (names1[0] == NULL && names2[0] == NULL) {
        return true;
    }
    if (names1[0] == NULL || names2[0] == NULL) {
        return false;
    }
    return strcmp(names1[0], names2[0]) == 0;
}
Piece *list_findPieceWithNetworkFlags(List *list, const char *n1name, NetworkFlags *n1flags, char *n2names[10], NetworkFlags *n2flags, Rotation *rot) {
    Piece *piece = NULL;
    Piece *bestPiece = NULL;
    int bestMatchCount = 0;
    int bestRot = 0;
    for (each_in_list(list, piece)) {
        if (strcmp(piece->network1Name, n1name) == 0) {
            for (int i = 0; i < (piece->simple ? 4 : 8); i++) {
                int matchCount = 0;
                if (pieceRotation_matches(piece->rotations[i], n1flags, n2flags, &matchCount)
                    && network2NamesCompatible(n2names, piece->network2Names)) {
                    if (matchCount > bestMatchCount) {
                        bestRot = i;
                        bestPiece = piece;
                        bestMatchCount = matchCount;
                    }
                }
            }
        }
    }
    if (bestPiece != NULL) {
        *rot = bestRot;
    }
    return bestPiece;
}

Piece::Piece(void) : Piece(0, NULL, false) {}

Piece::Piece(InstanceID tid, char *name, bool simple) {
    textureIID = tid;
    this->name = name;
    this->simple = simple;
    network1Name = NULL;
}

Piece::~Piece() {
    for (int i = 0; i < 8; i++) {
        delete this->rotations[i];
    }
}



Piece::Piece(Piece *piece) : Piece(piece->textureIID,
                                   piece->name,
                                   piece->simple) {
    for (int i = 0; i < 8; i++) {
        this->rotations[i] = new PieceRotation(piece->rotations[i]);
    }
}

Rotation rotation_next90C(Rotation *rot) {
    if (*rot == Rotation_270) {
        *rot = Rotation_0;
    } else {
        *rot += 1;
    }
    return *rot;
}

void Piece::fillRotationsFromBase(PieceRotation *baseRotation) {
    Piece *piece = this;
    Rotation rot = baseRotation->transformation.rotation;
    PieceRotation *t;
    int ma = baseRotation->transformation.flip == 1 ? 4 : 0; //mirror adjustment;
    piece->rotations[rot + ma] = new PieceRotation(baseRotation);
    for (int i = 0; i < 3; i++) {
        t = (new PieceRotation(piece->rotations[rot + ma]))->rotate90C();
        piece->rotations[rotation_next90C(&rot) + ma] = t;
    }
    int ma2 = baseRotation->transformation.flip == 1 ? 0 : 4; //anti mirror adjustment;
    for (int i = 0; i <= 3; i++) {
        piece->rotations[i + ma2] = (new PieceRotation(piece->rotations[i + ma]))->mirror();
    }
}

bool Piece::printRUL(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri) {
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



