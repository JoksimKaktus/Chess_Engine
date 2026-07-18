#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "gui/boardUpdate.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

int dragging = 0;
int mouseX, mouseY;

int init();
int loadMedia();
void closeWindow();

void drawBoard();
void drawPieces(); // Drawing the pieces on the board
void drawPiece(); // Draw one piece
void drawDraggedPiece(); // Draw the piece onto the cursor

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
							setDragFrom(x, y);

							// Check if clicked on a piece and remember the piece
							if (getCurBoard(x, y).type != 6) {
								setDragedPiece();
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
// gcc main.c -I src/include -L src/lib -o main.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image