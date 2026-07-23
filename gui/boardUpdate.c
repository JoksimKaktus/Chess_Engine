#include "boardUpdate.h"

Piece curBoard[64];

Coordinates dragFrom = (Coordinates){0,0};
Piece dragedPiece;
int enPassantCol = -1;
int castleRights[4] = {1,1,1,1};     // white kingside, white queenside, black kingside, black queenside

void updateBoard(int toX, int toY){
	curBoard[dragFrom.y*8 + dragFrom.x] = (Piece){6, 0};

    // delete the pawn when en passant
    if(curBoard[toY*8 + toX].type == 6 && dragedPiece.type == 5 && dragFrom.x != toX){
        curBoard[dragFrom.y*8 + toX] = (Piece){6, 0};
    }

    // Castling
    if(dragedPiece.type == 0 && abs(dragFrom.x - toX) == 2){
        if(dragFrom.x < toX){
            curBoard[dragFrom.y*8 + 7] = (Piece){6,0};
            curBoard[dragFrom.y*8 + 5] = (Piece){4, dragedPiece.color};
            castleRights[2*dragedPiece.color] = 0;
            castleRights[2*dragedPiece.color+1] = 0;
        }else{
            curBoard[dragFrom.y*8] = (Piece){6,0};
            curBoard[dragFrom.y*8 + 3] = (Piece){4, dragedPiece.color};
            castleRights[2*dragedPiece.color] = 0;
            castleRights[2*dragedPiece.color+1] = 0;
        }
    }

    curBoard[toY*8 + toX] = dragedPiece;

    // en passant control
    enPassantCol = -1;
    if(dragedPiece.type == 5 && abs(dragFrom.y - toY) == 2){
        enPassantCol = toX;
    }

    // removing castling rights
    if(dragedPiece.type == 0){
        castleRights[2*dragedPiece.color] = 0;
        castleRights[2*dragedPiece.color+1] = 0;
    }else if(dragedPiece.type == 4){
        if(dragFrom.y == 0){
            if(dragFrom.x == 0){
                castleRights[3] = 0;
            }else if(dragFrom.x == 7){
                castleRights[2] = 0;
            }
        }else if(dragFrom.y == 7){
            if(dragFrom.x == 0){
                castleRights[1] = 0;
            }else if(dragFrom.x == 7){
                castleRights[0] = 0;
            }
        }
    }
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
		curBoard[i] = (Piece){6,1};
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
		curBoard[i] = (Piece){6,0};
	}
}

Piece getCurBoard(int x, int y){
    return curBoard[y*8 + x];
}

void setCurBoard(Coordinates pos, Piece piece){
    curBoard[pos.y*8 + pos.x] = piece;
}

void setDragedPiece(){
    dragedPiece = getCurBoard(dragFrom.x,dragFrom.y);
}

void setDragedPiecePromotion(Piece piece){
    dragedPiece = piece;
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

int getEnPassantCol(){
    return enPassantCol;
}

int* getCastleRights(){
    return castleRights;
}