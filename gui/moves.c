#include "moves.h"

int addX[8] = {-1,0,1,0,-1,1,1,-1};
int addY[8] = {0,-1,0,1,-1,-1,1,1};
int addXKnight[8] = {-2,-2,-1,1,2,2,1,-1};
int addYKnight[8] = {1,-1,-2,-2,-1,1,2,2}; 

void expand(int addInd, int *ind, Coordinates moves[], Coordinates pos, int span, int color){
    for(int i = 0;i < span;i++){
        pos.x += addX[addInd];
        pos.y += addY[addInd];
        if(pos.x < 0 || pos.x >= 8 || pos.y < 0 || pos.y >= 8){
            break;
        }
        moves[*ind] = (Coordinates){pos.x, pos.y};
        *ind = *ind + 1;
        if(getCurBoard(pos.x, pos.y).type != 6){
            if(getCurBoard(pos.x, pos.y).color == color){
                *ind = *ind - 1;
            }
            break;
        }
    }
}

int getAllMoves(int color, Coordinates allMoves[], int checkCheck){
    int ind = 0;
    for(int i = 0;i < 8;i++){
        for(int j = 0;j < 8;j++){
            Piece piece = getCurBoard(j,i);
            if(piece.type != 6 && piece.color == color){
                ind = highlightPieceMoves(allMoves, piece, (Coordinates){j,i}, ind, checkCheck);
            }
        }
    }
    return ind;
}

int highlightPieceMoves(Coordinates moves[], Piece piece, Coordinates pos, int startInd, int checkCheck){
    int ind = startInd;
    if(piece.type == 3){
        for(int i = 0;i < 8;i++){
            if((pos.x + addXKnight[i] >= 0 && pos.x + addXKnight[i] < 8) && 
               (pos.y + addYKnight[i] >= 0 && pos.y + addYKnight[i] < 8) && 
               ((getCurBoard(pos.x + addXKnight[i], pos.y + addYKnight[i]).type) == 6 || (getCurBoard(pos.x + addXKnight[i], pos.y + addYKnight[i]).color) != piece.color)){
                moves[ind] = (Coordinates){pos.x + addXKnight[i],pos.y + addYKnight[i]};
                ind++;
            }
        }
    }else if(piece.type == 5){
        if(piece.color == 0){
            if(getCurBoard(pos.x, pos.y-1).type == 6){
                moves[ind] = (Coordinates){pos.x, pos.y-1};
                ind++;
                if(pos.y == 6 && getCurBoard(pos.x, pos.y-2).type == 6){
                    moves[ind] = (Coordinates){pos.x, pos.y-2};
                    ind++;
                }
            }
            if(pos.x != 0 && 
                ((getCurBoard(pos.x-1, pos.y-1).type != 6 && getCurBoard(pos.x-1, pos.y-1).color != piece.color) ||
                (pos.y == 3 && pos.x-1 == getEnPassantCol()))
            ){
                moves[ind] = (Coordinates){pos.x-1, pos.y-1};
                ind++;
            }
            if(pos.x != 7 &&
                ((getCurBoard(pos.x+1, pos.y-1).type != 6 && getCurBoard(pos.x+1, pos.y-1).color != piece.color) ||
                (pos.y == 3 && pos.x+1 == getEnPassantCol()))
            ){
                moves[ind] = (Coordinates){pos.x+1, pos.y-1};
                ind++;
            }
        }else{
            if(getCurBoard(pos.x, pos.y+1).type == 6){
                moves[ind] = (Coordinates){pos.x, pos.y+1};
                ind++;
                if(pos.y == 1 && getCurBoard(pos.x, pos.y+2).type == 6){
                    moves[ind] = (Coordinates){pos.x, pos.y+2};
                    ind++;
                }
            }
            if(pos.x != 0 && 
                ((getCurBoard(pos.x-1, pos.y+1).type != 6 && getCurBoard(pos.x-1, pos.y+1).color != piece.color) ||
                (pos.y == 4 && pos.x-1 == getEnPassantCol()))
            ){
                moves[ind] = (Coordinates){pos.x-1, pos.y+1};
                ind++;
            }
            if(pos.x != 7 &&
                ((getCurBoard(pos.x+1, pos.y+1).type != 6 && getCurBoard(pos.x+1, pos.y+1).color != piece.color) ||
                (pos.y == 4 && pos.x+1 == getEnPassantCol()))
            ){
                moves[ind] = (Coordinates){pos.x+1, pos.y+1};
                ind++;
            }
        }
    }else{
        // 0 - king, 1 - queen, 2 - bishop, 4 - rook
        int start = 0;
        int end = 8;
        int span = 8;
        if(piece.type == 2){
            start = 4;
        }else if(piece.type == 4){
            end = 4;
        }else if(piece.type == 0){
            span = 1;
        }
        for(int i = start;i < end;i++){
            expand(i, &ind, moves, pos, span, piece.color);
        }
        if(piece.type == 0){
            int *castleRights = getCastleRights();
            int kingside = castleRights[piece.color*2];
            int queenside = castleRights[piece.color*2 + 1];
            if(kingside && (getCurBoard(pos.x+1,pos.y).type == 6 && getCurBoard(pos.x+2,pos.y).type == 6)){
                moves[ind] = (Coordinates){pos.x+2,pos.y};
                ind++;
            }
            if(queenside && (getCurBoard(pos.x-1,pos.y).type == 6 && getCurBoard(pos.x-2,pos.y).type == 6 && getCurBoard(pos.x-3,pos.y).type == 6)){
                moves[ind] = (Coordinates){pos.x-2,pos.y};
                ind++;
            }
        }
    }
    if(checkCheck){
        for(int i = startInd;i < ind;i++){
            setCurBoard(pos, (Piece){6,0});
            Piece removed = getCurBoard(moves[i].x, moves[i].y);
            setCurBoard(moves[i], piece);
            Coordinates allMovesOpponent[256];
            int size = getAllMoves(piece.color ^ 1, allMovesOpponent, 0);
            int flag = 0;
            for(int j = 0; j < size;j++){
                if(getCurBoard(allMovesOpponent[j].x, allMovesOpponent[j].y).type == 0){
                    flag = 1;
                    break;
                }
            }
            setCurBoard(moves[i], removed);
            setCurBoard(pos, piece);
            if(flag){
                ind--;
                moves[i] = moves[ind];
                i--;
            }
        }
    }
    return ind;
}