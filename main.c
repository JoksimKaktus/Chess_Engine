#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

typedef struct {
    int type;  // 0 - king, 1 - queen, 2 - bishop, 3 - knight, 4 - rook, 5 - pawn
    int color; // 0 - white, 1 - black
} Piece;


char curBoard[128] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/";
uint64_t piecesBitBoard = 0xFFFF00000000FFFF;

int dragging = 0;
int DragFromX, DragFromY;
int mouseX, mouseY;
Piece dragedPiece;

int init();
int loadMedia();
void closeWindow();

void drawBoard();
void drawPieces(); // Drawing the pieces on the board
void drawPiece(); // Draw one piece
void drawDraggedPiece(); // Draw the piece onto the cursor
Piece findPieceAt(); // Find the piece at coordinates(...)
Piece findPieceByNotation(char c); // Find piece by its notation in FEN
char findNotationByPiece();
void updateBoard();


SDL_Surface* loadSurface( char* path );
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* piecesTexture = NULL;

int init()
{
	//Initialization flag
	int success = 1;

	//Initialize SDL
	if	( SDL_Init( SDL_INIT_VIDEO ) < 0 )	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = 0;
	}
	else	{
		//Create window
		gWindow =
			SDL_CreateWindow(
				"Chess",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
				SDL_WINDOW_SHOWN
			);

		if	( gWindow == NULL )	{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = 0;
		}
		else	{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if	( gRenderer == NULL )	{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = 0;
			}else	{
				// Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
					printf("SDL_image could not initialize! %s\n", IMG_GetError());
					return 0;
				}
			}
		}
	}

	return success;
}

int loadMedia()
{
	//Loading success flag
	int success = 1;

	piecesTexture = IMG_LoadTexture(gRenderer, "images/pieces.png");
	if (piecesTexture == NULL) {
		printf("Unable to load texture! %s\n", IMG_GetError());
	}
	
	return success;
}

void closeWindow()
{
	// Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void drawBoard(){
	for(int i = 0;i < 8;i++){
		for(int j = 0;j < 8;j++){
			SDL_Rect fillRect = { SCREEN_WIDTH/8 * j, SCREEN_WIDTH/8 * i, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 };
			if((i + j) % 2 == 0){
				SDL_SetRenderDrawColor( gRenderer, 250, 160, 90, 255 );     
			}else{
				SDL_SetRenderDrawColor( gRenderer, 165, 90, 30, 255 );
			}        
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
}

void drawPieces(){
	int i = 0;
	int j = 0;
	int ind = 0;
	while(!(i == 7 && j == 8)){
		if(curBoard[ind] == '/'){
			i++;
			j = 0;
		}else if(curBoard[ind] >= '0' && curBoard[ind] <= '9'){
			j += curBoard[ind] - '0'; 
		}else{
			if (dragging && i == DragFromY && j == DragFromX){
				j++;
				ind++;
				continue;
			}
			int color = 1;
			char piece = curBoard[ind];
			if(curBoard[ind] <= 'Z'){
				color = 0;
				piece += 'a' - 'A';
			}
			if(piece == 'p'){
				drawPiece(5,color,j,i);
			}else if(piece == 'r'){
				drawPiece(4,color,j,i);
			}else if(piece == 'n'){
				drawPiece(3,color,j,i);
			}else if(piece == 'b'){
				drawPiece(2,color,j,i);
			}else if(piece == 'q'){
				drawPiece(1,color,j,i);
			}else{
				drawPiece(0,color,j,i);
			}
			j++;
		}

		ind++;
	}
}

void drawPiece(int spriteX, int spriteY, int boardX, int boardY)
{
	int size = 107;
    SDL_Rect src = {
        spriteX * size,
        spriteY * size,
        size,
        size
    };

    int s = SCREEN_WIDTH / 8;

    SDL_Rect dst = {
        boardX * s,
        boardY * s,
        s,
        s
    };

    SDL_RenderCopy(gRenderer, piecesTexture, &src, &dst);
}

void drawDraggedPiece(int spriteX, int spriteY, int mouseX, int mouseY)
{
    int squareSize = SCREEN_WIDTH / 8;

	int size = 107;
	SDL_Rect src = {
        spriteX * size,
        spriteY * size,
        size,
        size
    };

    SDL_Rect dst = {
        mouseX - squareSize / 2,
        mouseY - squareSize / 2,
        squareSize,
        squareSize
    };

    SDL_RenderCopy(gRenderer, piecesTexture, &src, &dst);
}

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


	printf("\n\n%s\n\n", curBoard);
}

int main( int, char* [] )
{
	// Start up SDL and create window
	if	( !init() )	{
		printf( "Failed to initialize!\n" );
	}
	else	{
		// Load media
		if	( !loadMedia() )	{
			printf( "Failed to load media!\n" );
		}
		else	{
			int quit = 0;

			// Event handler
			SDL_Event e;

			// While application is running
			while ( !quit )
			{
				// Handle events on queue
				while ( SDL_PollEvent( &e ) != 0 )	{
					if	(e.type == SDL_QUIT)	{
						quit = 1;
					}else if (e.type == SDL_MOUSEBUTTONDOWN){
						 if (e.button.button == SDL_BUTTON_LEFT) {
							// Get mouse and chessboard coordinates
							mouseX = e.button.x;
							mouseY = e.button.y;

							DragFromX = mouseX / (SCREEN_WIDTH / 8);
							DragFromY = mouseY / (SCREEN_HEIGHT / 8);

							// Check if clicked on a piece and remember the piece
							if (piecesBitBoard & (1ULL << (DragFromY*8 + DragFromX)) ) {
								dragedPiece = findPieceAt(DragFromX, DragFromY);
								dragging = 1;
							}
						}
					}else if(e.type == SDL_MOUSEMOTION){
						// Update the mouse cooridnates
						mouseX = e.motion.x;
						mouseY = e.motion.y;
					}else if(e.type == SDL_MOUSEBUTTONUP){
						if (dragging) {
							int toX = e.button.x / (SCREEN_WIDTH / 8);
							int toY = e.button.y / (SCREEN_HEIGHT / 8);

							// validate move TODO

							// update board
							updateBoard(toX,toY);

							dragging = 0;
						}
					}
				}

				//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );


				drawBoard();
				drawPieces();

				if (dragging) {
					drawDraggedPiece(dragedPiece.type, dragedPiece.color, mouseX, mouseY);
				}
								

				// Update the surface
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	// Free resources and close SDL
	closeWindow();

	return 0;
}
// gcc main.c -I src/include -L src/lib -o main.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image