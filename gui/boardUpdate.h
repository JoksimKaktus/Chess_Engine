#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef BOARDUPDATE_H
#define BOARDUPDATE_H

typedef struct {
    int type;  // 0 - king, 1 - queen, 2 - bishop, 3 - knight, 4 - rook, 5 - pawn
    int color; // 0 - white, 1 - black
} Piece;

extern char curBoard[128];
extern uint64_t piecesBitBoard;

extern int DragFromX;
extern int DragFromY;
extern Piece dragedPiece;

Piece findPieceAt(int boardX, int boardY); // Find the piece at coordinates(...)
Piece findPieceByNotation(char c); // Find piece by its notation in FEN
char findNotationByPiece(Piece piece);
void updateBoard(int toX, int toY);

#endif