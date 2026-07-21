#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef BOARDUPDATE_H
#define BOARDUPDATE_H

typedef struct {
    int type;  // 0 - king, 1 - queen, 2 - bishop, 3 - knight, 4 - rook, 5 - pawn, 6 - nothing
    int color; // 0 - white, 1 - black
} Piece;

typedef struct {
    int x;
    int y;
} Coordinates;

void updateBoard(int toX, int toY);
void initBoard();
Piece getCurBoard(int x, int y);
void setCurBoard(Coordinates pos, Piece piece);
void setDragedPiece();
Piece getDragedPiece();
Coordinates setDragFrom(int x, int y);
Coordinates getDragFrom();
int getEnPassantCol();
int* getCastleRights();

#endif