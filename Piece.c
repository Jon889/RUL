#include "Piece.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "PieceRotation.h"
#include "NetworkFlags.h"

void piece_printAsciiArt(Piece *p, int rotation) {
    NetworkFlags *piece = p->rotations[rotation]->network1;
    printf("+-----+\n");
    printf("|  %2d |\n", piece->north);
    printf("|%2d %2d|\t0x%08x,%i,%i %s\n", piece->west, piece->east, p->textureIID, p->rotations[rotation]->transformation.rotation, p->rotations[rotation]->transformation.flip, p->name);
    printf("|  %2d |\n", piece->south);
    printf("+-----+\n");
}
void piece_printAsciiArtPair(Piece *piece, int rotation1, Piece *p2, int rotation2) {
    NetworkFlags *nf1 = piece->rotations[rotation1]->network1;
    NetworkFlags *nf2 = p2->rotations[rotation2]->network1;
    printf("+-----+-----+\n");
    printf("|  %2d |  %2d |\n", nf1->north, nf2->north);
    printf("|%2d %2d|%2d %2d|\n", nf1->west, nf1->east, nf2->west, nf2->east);
    printf("|  %2d |  %2d |\n", nf1->south, nf2->south);
    printf("+-----+-----+\n");
}



#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


Piece * piece_createEmpty(void) {
    return piece_create(0, NULL, false);
}
Piece * piece_create(InstanceID tid, char *name, bool simple) {
    Piece *p = calloc(1, sizeof(Piece));
    p->textureIID = tid;
    p->name = name;
    p->simple = simple;
    return p;
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

void piece_printRUL(Piece *ll, int lli, Piece *lr, int lri, Piece *rl, int rli, Piece *rr, int rri) {
    printf("0x%08x,%i,%i,0x%08x,%i,%i",
           ll->textureIID, ll->rotations[lli]->transformation.rotation, ll->rotations[lli]->transformation.flip,
           lr->textureIID, lr->rotations[lri]->transformation.rotation, lr->rotations[lri]->transformation.flip);
    if (rr != NULL) {
        printf("=0x%08x,%i,%i,0x%08x,%i,%i",
               rl->textureIID, rl->rotations[rli]->transformation.rotation, rl->rotations[rli]->transformation.flip,
               rr->textureIID, rr->rotations[rri]->transformation.rotation, rr->rotations[rri]->transformation.flip);
    }
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



int piece_printRules(Piece *leftPiece, Map *map, List *list) {
    //Left piece is 1st Argument, Right piece is gotten by going through every piece is map
    MappingValue *mapValue = NULL;
    int totalRulsPrinted = 0;
    for (each_in_list(map->mapping, mapValue)) {
        Piece *rightPiece = mapValue->basePiece;
        Piece *rightOverridePiece = mapValue->overridePiece;
        int commentLength = printf(" ;%s %s -> %s %s", map->baseName, rightPiece->name, map->overrideName, rightPiece->name);
        int rulsPrinted = piece_printRulesOfPieces(leftPiece, rightPiece, rightOverridePiece, false, false);
        totalRulsPrinted += rulsPrinted;
        if (rulsPrinted == 0) {
            printf("\r%*s\r", commentLength, "");
        } else {
            printf("\n");
        }
    }
    return totalRulsPrinted;
}
