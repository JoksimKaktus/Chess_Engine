#include "boardUpdate.h"

char curBoard[128] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/";
uint64_t piecesBitBoard = 0xFFFF00000000FFFFULL;

int DragFromX = 0;
int DragFromY = 0;
Piece dragedPiece;

Piece findPieceAt(int boardX, int boardY){
	int i = 0;
	int j = 0;
	int ind = 0;
	while(!(i == 7 && j == 8)){
		if(i == boardY && j == boardX){
			break;
		}
		if(curBoard[ind] == '/'){
			i++;
			j = 0;
		}else if(curBoard[ind] >= '0' && curBoard[ind] <= '9'){
			j += curBoard[ind] - '0'; 
		}else{
			j++;
		}

		ind++;
	}

	return findPieceByNotation(curBoard[ind]);
}

Piece findPieceByNotation(char c){
	int color = 1;
	char piece = c;
	if(c <= 'Z'){
		color = 0;
		piece += 'a' - 'A';
	}
	Piece ret = {0, color};
	if(piece == 'p'){
		ret.type = 5;
	}else if(piece == 'r'){
		ret.type = 4;
	}else if(piece == 'n'){
		ret.type = 3;
	}else if(piece == 'b'){
		ret.type = 2;
	}else if(piece == 'q'){
		ret.type = 1;
	}
	return ret;
}

char findNotationByPiece(Piece piece){
	char ret = 'k';
	if(piece.type == 5){
		ret = 'p';
	}else if(piece.type == 4){
		ret = 'r';
	}else if(piece.type == 3){
		ret = 'n';
	}else if(piece.type == 2){
		ret = 'b';
	}else if(piece.type == 1){
		ret = 'q';
	}
	if(piece.color == 0){
		ret -= 'a' - 'A';
	}
	
	return ret;
}

void updateBoard(int toX, int toY){
	piecesBitBoard ^= (1ULL << (DragFromY*8 + DragFromX));
	piecesBitBoard |= (1ULL << (toY*8 + toX));

	char newBoard[128] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	int indOld = 0;
	int indNew = 0;
	for(int i = 0;i < 8;i++){
		if(i != DragFromY){
			while(curBoard[indOld] != '/'){
				newBoard[indNew] = curBoard[indOld];
				indNew++;
				indOld++;
			}
			indOld++;
		}else{
			int j = 0;
			int numBefore = 0;
			int add = 0;
			while(curBoard[indOld] != '/'){
				if(j == DragFromX){
					if(curBoard[indOld+1] >= '0' && curBoard[indOld+1] <= '9'){
						if(numBefore){
							newBoard[indNew-1] += curBoard[indOld+1] - '0' + 1;
						}else{
							newBoard[indNew] = curBoard[indOld+1] + 1;
							indNew++;
						}
						j += curBoard[indOld] - '0';
						indOld += 2;
					}else{
						if(numBefore){
							newBoard[indNew-1] += 1;
						}else{
							newBoard[indNew] = '1';
							indNew++;
						}
						indOld++;
						j++;
					}
				}else{
					if(curBoard[indOld] >= '0' && curBoard[indOld] <= '9'){
						newBoard[indNew] = curBoard[indOld] + add;
						j += curBoard[indOld] - '0';
						numBefore = curBoard[indOld] - '0';
						indNew++;
						indOld++;
					}else{
						newBoard[indNew] = curBoard[indOld];
						indNew++;
						indOld++;
						numBefore = 0;
						j++;
					}
				}
			}
			indOld++;
		}
		newBoard[indNew] = '/';
		indNew++;
	}
	newBoard[indNew] = '\0';
	strcpy(curBoard, newBoard);
	
	strcpy(newBoard,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	indOld = 0;
	indNew = 0;
	for(int i = 0;i < 8;i++){
		if(i != toY){
			while(curBoard[indOld] != '/'){
				newBoard[indNew] = curBoard[indOld];
				indNew++;
				indOld++;
			}
			indOld++;
		}else{
			int j = 0;
			while(curBoard[indOld] != '/'){
				if(curBoard[indOld] >= '0' && curBoard[indOld] <= '9'){
					if(j <= toX && toX < j + curBoard[indOld] - '0'){
						int before = toX - j;
						int after = j + curBoard[indOld] - '0' - toX - 1;
						if(before != 0){
							newBoard[indNew] = '0' + before;
							indNew++;
						}
						newBoard[indNew] = findNotationByPiece(dragedPiece);
						indNew++;
						if(after != 0){
							newBoard[indNew] = '0' + after;
							indNew++;
						}
						j += curBoard[indOld] - '0';
						indOld++;
					}else{
						j += curBoard[indOld] - '0';
						newBoard[indNew] = curBoard[indOld];
						indOld++;
						indNew++;
					}
				}else{
					if(j == toX){
						newBoard[indNew] = findNotationByPiece(dragedPiece);
					}else{
						newBoard[indNew] = curBoard[indOld];
					}
					indNew++;
					indOld++;
					j++;
				}
			}
			indOld++;
		}
		newBoard[indNew] = '/';
		indNew++;
	}
	newBoard[indNew] = '\0';
	strcpy(curBoard, newBoard);
}
