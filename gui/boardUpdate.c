#include "boardUpdate.h"

Piece curBoard[64];

uint64_t piecesBitBoard = 0xFFFF00000000FFFF;
Coordinates dragFrom = (Coordinates){0,0};
Piece dragedPiece;

void updateBoard(int toX, int toY){
	piecesBitBoard ^= (1ULL << (dragFrom.y*8 + dragFrom.x));
	piecesBitBoard |= (1ULL << (toY*8 + toX));

	curBoard[dragFrom.y*8 + dragFrom.x] = (Piece){6, 0};
	curBoard[toY*8 + toX] = dragedPiece;
}

void initBoard(){
    for(int i = 16; i < 48; i++){
        curBoard[i] = (Piece){6, 0};
    }

    curBoard[0] = (Piece){4,1};
    curBoard[1] = (Piece){3,1};
    curBoard[2] = (Piece){2,1};
    curBoard[3] = (Piece){1,1};
    curBoard[4] = (Piece){0,1};
    curBoard[5] = (Piece){2,1};
    curBoard[6] = (Piece){3,1};
    curBoard[7] = (Piece){4,1};

    for(int i = 8; i < 16; i++){
		curBoard[i] = (Piece){5,1};
	}

    curBoard[56] = (Piece){4,0};
    curBoard[57] = (Piece){3,0};
    curBoard[58] = (Piece){2,0};
    curBoard[59] = (Piece){1,0};
    curBoard[60] = (Piece){0,0};
    curBoard[61] = (Piece){2,0};
    curBoard[62] = (Piece){3,0};
    curBoard[63] = (Piece){4,0};

    for(int i = 48; i < 56; i++){
		curBoard[i] = (Piece){5,0};
	}
}

Piece getCurBoard(int x, int y){
    return curBoard[y*8 + x];
}

void setDragedPiece(){
    dragedPiece = getCurBoard(dragFrom.x,dragFrom.y);
}

Piece getDragedPiece(){
    return dragedPiece;
}

Coordinates setDragFrom(int x, int y){
    dragFrom.x = x;
    dragFrom.y = y;
}

Coordinates getDragFrom(){
    return dragFrom;
}