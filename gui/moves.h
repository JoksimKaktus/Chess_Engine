#include <stdio.h>

#include "boardUpdate.h"

#ifndef MOVES_H
#define MOVES_H

int getAllMoves(int color, Coordinates allMoves[], int checkCheck);
int highlightPieceMoves(Coordinates moves[], Piece piece, Coordinates pos, int startInd, int checkCheck);

#endif