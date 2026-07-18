#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef BOARDUPDATE_H
#define BOARDUPDATE_H

typedef struct {
    int type;  // 0 - king, 1 - queen, 2 - bishop, 3 - knight, 4 - rook, 5 - pawn, 6 - nothing
    int color; // 0 - white, 1 - black
} Piece;

extern Piece curBoard[64];
extern uint64_t piecesBitBoard;

extern int DragFromX;
extern int DragFromY;
extern Piece dragedPiece;

void updateBoard(int toX, int toY);
void initBoard();

#endif