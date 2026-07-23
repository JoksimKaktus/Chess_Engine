#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "gui/boardUpdate.h"
#include "gui/moves.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

int move = 0;

int dragging = 0;
int selected = 0;
int mouseX, mouseY;
int highlightSize = 0;
Coordinates highlight[256];

int promotionPending = 0;
Coordinates promotionSquare;
Piece promotionPiece;

int init();
int loadMedia();
void closeWindow();

void drawBoard();
void drawPieces(); // Drawing the pieces on the board
void drawPiece(); // Draw one piece
void drawDraggedPiece(); // Draw the piece onto the cursor
void drawPromotion();
int validMove(int toX, int toY);
void makeMove(int toX, int toY);
void findPromotionPiece(int x, int y);

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
			SDL_Rect fillRect = { SCREEN_WIDTH/8 * j, SCREEN_HEIGHT/8 * i, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 };
			if((i + j) % 2 == 0){
				SDL_SetRenderDrawColor( gRenderer, 250, 160, 90, 255 );     
			}else{
				SDL_SetRenderDrawColor( gRenderer, 165, 90, 30, 255 );
			}        
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
	if(!dragging && !selected) return;
	for(int i = 0;i < highlightSize;i++){
		SDL_Rect fillRect = { SCREEN_WIDTH/8 * highlight[i].x, SCREEN_HEIGHT/8 * highlight[i].y, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 };
		if((highlight[i].x + highlight[i].y) % 2 == 0){
			SDL_SetRenderDrawColor( gRenderer, 255, 100, 40, 255 );  
		}else{
			SDL_SetRenderDrawColor( gRenderer, 195, 60, 10, 255 );
		}
		SDL_RenderFillRect( gRenderer, &fillRect );
	}
}

void drawPieces(){
	for(int i = 0;i < 8;i++){
		for(int j = 0;j < 8;j++){
			if(getCurBoard(j,i).type != 6){
				Coordinates dragFrom = getDragFrom(); 
				if (dragging && i == dragFrom.y && j == dragFrom.x){
					continue;
				}
				drawPiece(getCurBoard(j,i).type,getCurBoard(j,i).color,j,i);
			}
		}
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

void drawDraggedPiece(int mouseX, int mouseY)
{
	Piece piece = getDragedPiece();
    int squareSize = SCREEN_WIDTH / 8;

	int size = 107;
	SDL_Rect src = {
        piece.type * size,
        piece.color * size,
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

void drawPromotion(){
	for(int i = 2;i < 5;i++){
		for(int j = 1; j < 7;j++){
			SDL_Rect fillRect = { SCREEN_WIDTH/8 * j, SCREEN_HEIGHT/8 * i, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 };
			if(i == 3 && j != 1 && j != 6){
				SDL_SetRenderDrawColor( gRenderer, 180, 180, 180, 255 );
			}else{
				SDL_SetRenderDrawColor( gRenderer, 230, 230, 230, 255 );
			}
			SDL_RenderFillRect( gRenderer, &fillRect );
		}
	}
	drawPiece(1, move, 2, 3);
	drawPiece(2, move, 3, 3);
	drawPiece(3, move, 4, 3);
	drawPiece(4, move, 5, 3);
}

int validMove(int toX, int toY){
	for(int i = 0;i < highlightSize;i++){
		if(highlight[i].x == toX && highlight[i].y == toY){
			return 1;
		}
	}
	return 0;
}

void makeMove(int toX, int toY){
	if(getDragedPiece().type == 5 && (toY == 0 || toY == 7)){ // Promotion
		promotionPending = 1;
		promotionSquare = (Coordinates){toX, toY};
		selected = 0;
		dragging = 0;
		return;
	}
	updateBoard(toX,toY);
	Coordinates numOfMoves[256];
	move ^= 1;
	int ind = getAllMoves(move, numOfMoves, 1);
	if(ind == 0){
		printf("GAME OVER\n");
		gameOver(move^1);
	}
}

void findPromotionPiece(int x, int y){
	if(x > 1 && x < 6 && y == 3){
		promotionPending = 0;
		setDragedPiecePromotion((Piece){x-1,move});
		makeMove(promotionSquare.x, promotionSquare.y);
	}
}

int main( int, char* [] )
{
	// Start up SDL and create window
	if	( !init() )	{
		printf("Failed to initialize!\n");
	}
	else	{
		// Load media
		if	( !loadMedia() )	{
			printf("Failed to load media!\n");
		}
		else	{
			int quit = 0;

			// Event handler
			SDL_Event e;

			initBoard();

			// While application is running
			while ( !quit )
			{
				if(promotionPending){
					while ( SDL_PollEvent( &e ) != 0 )	{
						if	(e.type == SDL_QUIT)	{
							quit = 1;
						}else if(e.type == SDL_MOUSEBUTTONUP){
							mouseX = e.button.x;
							mouseY = e.button.y;

							int x = mouseX / (SCREEN_WIDTH / 8);
							int y = mouseY / (SCREEN_HEIGHT / 8);

							findPromotionPiece(x, y);
						}
					}
				}else{
					// Handle events on queue
					while ( SDL_PollEvent( &e ) != 0 )	{
						if	(e.type == SDL_QUIT)	{
							quit = 1;
						}else if (e.type == SDL_MOUSEBUTTONDOWN){
							if (e.button.button == SDL_BUTTON_LEFT) {
								// Get mouse and chessboard coordinates
								mouseX = e.button.x;
								mouseY = e.button.y;

								int x = mouseX / (SCREEN_WIDTH / 8);
								int y = mouseY / (SCREEN_HEIGHT / 8);

								// Check if clicked on a piece and remember the piece
								if (!selected && getCurBoard(x, y).type != 6 && getCurBoard(x, y).color == move) {
									setDragFrom(x, y);
									setDragedPiece();
									dragging = 1;
									highlightSize = highlightPieceMoves(highlight, getDragedPiece(), getDragFrom(), 0, 1);
								}
							}
						}else if(e.type == SDL_MOUSEMOTION){
							// Update the mouse cooridnates
							mouseX = e.motion.x;
							mouseY = e.motion.y;
						}else if(e.type == SDL_MOUSEBUTTONUP){
							int toX = e.button.x / (SCREEN_WIDTH / 8);
							int toY = e.button.y / (SCREEN_HEIGHT / 8);
							if (dragging) {
								int isValid = validMove(toX, toY);
								if(isValid){
									makeMove(toX, toY);
									selected = 0;
								}else{
									Coordinates dragFrom = getDragFrom();
									if(dragFrom.x == toX && dragFrom.y == toY){
										selected = 1;
									}else{
										selected = 0;
									}
								}
								dragging = 0;
							}else if(selected){
								int isValid = validMove(toX, toY);
								if(isValid){
									Coordinates dragFrom = getDragFrom();
									if(!(dragFrom.x == toX && dragFrom.y == toY)){
										makeMove(toX, toY);
									}
								}
								selected = 0;
							}
							
						}
					}
				}

				//Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );


				drawBoard();
				drawPieces();
				if(promotionPending){
					drawPromotion();
				}

				if (dragging) {
					drawDraggedPiece(mouseX, mouseY);
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
// gcc main.c gui/boardUpdate.c gui/moves.c -I src/include -L src/lib -o main.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

/*
Stalemate
Dead position
50 - rules
3 times same pos
Time
*/