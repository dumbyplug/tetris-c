#include <SDL2/SDL.h>
#include <stdio.h> 
#include <time.h> 

#define WINWIDTH 800
#define WINHEIGHT 800

#define GAMEX 400
#define GAMEY 800

#define BLOCKSIZE 40

#define GRIDX (GAMEX / BLOCKSIZE)
#define GRIDY (GAMEY / BLOCKSIZE)


int move(SDL_Rect *rect, int gameGrid[GRIDY][GRIDX], int deltaX){
	int i, x, y;
	for(i = 0; i < 4; i++){
		x = (rect + i)->x + deltaX;
		y = (rect + i)->y;

		if((x >= GAMEX) || (x < 0))
			return 1;
		if(y >= 0)
			if(gameGrid[y / BLOCKSIZE][x / BLOCKSIZE])
				return 2;
	}

	for(i = 0; i < 4; i++)
		(rect + i)->x += deltaX;
	return 0;
}
int fall(SDL_Rect *rect,  int deltaY, int gameGrid[GRIDY][GRIDX]){
	int i, x, y;

	for(i = 0; i < 4; i++){
		x = (rect + i)->x;
		y = (rect + i)->y + deltaY;

		if(y >= GAMEY)
			return 1; 
		if(y >= 0)
			if(gameGrid[y / BLOCKSIZE][x / BLOCKSIZE])
				return 2;
	}
	
	for(i = 0; i < 4; i++)
		(rect + i)->y += deltaY;
	return 0;
}

void draw_rectangle(SDL_Renderer *renderer, SDL_Rect *rect,int r, int g, int b){
	SDL_Rect temp;

	int pixelSize = BLOCKSIZE / 8;

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderFillRect(renderer, rect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	temp.w = BLOCKSIZE; temp.h = pixelSize;
	temp.x = rect->x  ; temp.y = rect->y;
	SDL_RenderFillRect(renderer, &temp);
	temp.x = rect->x  ; temp.y = rect->y + BLOCKSIZE - pixelSize;
	SDL_RenderFillRect(renderer, &temp);
	
	temp.w = pixelSize; temp.h = BLOCKSIZE;
	temp.x = rect->x  ; temp.y = rect->y;
	SDL_RenderFillRect(renderer, &temp);
	temp.x = rect->x + 7 * BLOCKSIZE / 8; temp.y = rect->y;
	SDL_RenderFillRect(renderer, &temp);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	temp.w = pixelSize;           temp.h = pixelSize;
	temp.x = rect->x + pixelSize; temp.y = rect->y + pixelSize;
	SDL_RenderFillRect(renderer, &temp);

	temp.x = rect->x + 2 * pixelSize; temp.y = rect->y + 2 * pixelSize;
	SDL_RenderFillRect(renderer, &temp);

	temp.x = rect->x + 3 * pixelSize; temp.y = rect->y + 2 * pixelSize;
	SDL_RenderFillRect(renderer, &temp);

	temp.x = rect->x + 2 * pixelSize; temp.y = rect->y + 3 * pixelSize;
	SDL_RenderFillRect(renderer, &temp);

}

void draw_fallen_pieces(SDL_Renderer *renderer, int gameGrid[GRIDY][GRIDX]){
	SDL_SetRenderDrawColor(renderer, 23, 41, 51, 255);
	
	SDL_Rect temp;
	temp.w = BLOCKSIZE;
	temp.h = BLOCKSIZE;

	for(int i = 0; i < GRIDY; i++){
		for(int j = 0; j < GRIDX; j++){
			if(gameGrid[i][j]){
				temp.x = j * BLOCKSIZE; temp.y = i * BLOCKSIZE;
				//SDL_RenderFillRect(renderer, &temp);
				draw_rectangle(renderer, &temp, 0, 0, 255);
			}
		}
	}
}

void delete_full_lines(SDL_Renderer *renderer, int gameGrid[GRIDY][GRIDX]){
	int i, i2, j;
	char full_line;
	for(i = GRIDY - 1; i >= 0; i--){
		full_line = 1;
		for(j = 0; j < GRIDX; j++){
			if(!gameGrid[i][j]){
				full_line = 0;
			}
		}
		if(full_line){
			for(j = 0; j < GRIDX; j++){
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);
				gameGrid[i][j] = 0;
				draw_fallen_pieces(renderer, gameGrid);
				SDL_RenderPresent(renderer);
				SDL_Delay(30);
			}
			for(i2  = i; i2 > 0; i2--){
				for(j = 0; j < GRIDX; j++){
					gameGrid[i2][j] = gameGrid[i2 - 1][j];
				}
			}
			i = GRIDY;
		}
	}
}


int rotate(SDL_Rect *rect, int gameGrid[GRIDY][GRIDX], int piece, int rotation){
	int i, x, y;
	int preset[][6] = {
		{  0, -1,  1,  0,  1,  1},
		{  1,  0,  0,  1, -1,  1},
		{  0, -1,  1,  0,  1,  1},
		{  1,  0,  0,  1, -1,  1},

		{  1, -1,  1,  0,  0,  1},
		{ -1,  0,  0,  1,  1,  1},
		{  1, -1,  1,  0,  0,  1},
		{ -1,  0,  0,  1,  1,  1},

		{ -1,  0,  0,  1,  1,  0},
		{ -1,  0,  0,  1,  0, -1},
		{ -1,  0,  0, -1,  1,  0},
		{  0, -1,  1,  0,  0,  1},

		{ -1,  1, -1,  0,  1,  0},
		{ -1, -1,  0, -1,  0,  1},
		{ -1,  0,  1,  0,  1, -1},
		{  0, -1,  0,  1,  1,  1},

		{ -1,  0,  1,  0,  1,  1},
		{  0, -1,  0,  1, -1,  1},
		{ -1, -1, -1,  0,  1,  0},
		{  0, -1,  1, -1,  0,  1},

		{ -2,  0, -1,  0,  1,  0},
		{  0, -2,  0, -1,  0,  1},
		{ -2,  0, -1,  0,  1,  0},
		{  0, -2,  0, -1,  0,  1},

		{  1,  0,  0,  1,  1,  1},
		{  1,  0,  0,  1,  1,  1},
		{  1,  0,  0,  1,  1,  1},
		{  1,  0,  0,  1,  1,  1}
	};


	for(i = 0; i < 3; i++){
		x = (rect + 0)->x + preset[piece * 4 + rotation][2 * i] * BLOCKSIZE;
		y = (rect + 0)->y + preset[piece * 4 + rotation][2 * i + 1] * BLOCKSIZE;
		
		if((x >= GAMEX) || (x < 0) || (y >= GAMEY))
			return 1;

		if(y >= 0)
			if(gameGrid[y / 40][x / 40])
				return 2;
	}

	for(i = 0; i < 3; i++){
		(rect + i +1 )->x = (rect+0)->x + 
			preset[piece * 4 + rotation][2 * i] * BLOCKSIZE;

		(rect + i + 1)->y = (rect+0)->y + 
			preset[piece * 4 + rotation][2 * i + 1] * BLOCKSIZE;
	}
	return 0;
}

int main(){
	int i;
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Tetris", 
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINWIDTH, WINHEIGHT, 0);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event event;

	int gameGrid[GAMEY / BLOCKSIZE][GAMEX / BLOCKSIZE];
	SDL_Rect rect[4];

	for(i = 0; i < GAMEY / BLOCKSIZE; i++){
		for(int j = 0; j < GAMEX / BLOCKSIZE; j++){
			gameGrid[i][j] = 0;
		}
	}

	for(i = 0; i < 4; i++){
		rect[i].w = BLOCKSIZE; 
		rect[i].h = BLOCKSIZE; 
	}

	int piece = rand() % 7, rotation = rand() % 4;
	rect[0].x = GAMEX / 2 - rect[0].w;
	rect[0].y = -2 * BLOCKSIZE;
	rotate(rect, gameGrid, piece, rotation);

	SDL_Rect border;
	border.w = BLOCKSIZE; border.h = GAMEY;
	border.x = GAMEX; border.y = 0;

	char run = 1;
	int movePeriodForThisLevel = 20; 
	int movePeriod = movePeriodForThisLevel; 
	int movePeriodIndex = 0; 
	while(run){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				run = 0;
			}
			else if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_LEFT:
						move(rect, gameGrid, -BLOCKSIZE);
						break;
					case SDLK_RIGHT:
						move(rect, gameGrid, +BLOCKSIZE);
						break;

					case SDLK_z:
						rotation--;
						if(rotation < 0)
							rotation = 3;
						rotate(rect, gameGrid, piece, rotation);
						break;
					case SDLK_x:
						rotation++;
						if(rotation > 3)
							rotation = 0;
						rotate(rect, gameGrid, piece, rotation);
						break;
					case SDLK_SPACE:
						movePeriod = movePeriodForThisLevel / 9;
						break;
				}
			}
			
			else if(event.type  == SDL_KEYUP){
				switch(event.key.keysym.sym){
					case SDLK_SPACE:
						movePeriod = movePeriodForThisLevel;
						break;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// drawing falling pieces
		for(i = 0; i < 4; i++){
			draw_rectangle(renderer, rect + i, 255, 0, 0);
		}
	
		// handling piece move
		if(movePeriodIndex > movePeriod){
			movePeriodIndex = 0;
			if(fall(rect, BLOCKSIZE, gameGrid)){
				// updating grid
				for(i = 0; i < 4; i++){
					gameGrid[(rect + i)->y / BLOCKSIZE]
						[(rect + i)->x / BLOCKSIZE] = 1;
				}
				delete_full_lines(renderer, gameGrid);


				rect[0].x = GAMEX / 2 - rect[0].w;
				rect[0].y = -2 * BLOCKSIZE;
				
				piece = rand() % 7, rotation = rand() % 4;
				rotate(rect, gameGrid, piece, rotation);
			}
		}
		draw_fallen_pieces(renderer, gameGrid);

		SDL_SetRenderDrawColor(renderer, 65, 65, 65, 255);
		SDL_RenderFillRect(renderer, &border);

		SDL_RenderPresent(renderer);

		movePeriodIndex++;
		SDL_Delay(10);
	}
	return 0;
}

