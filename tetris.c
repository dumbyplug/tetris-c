#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

	// drawing sides and light effect
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
				draw_rectangle(renderer, &temp, 0, 0, 255);
			}
		}
	}
}


int delete_full_lines(int gameGrid[GRIDY][GRIDX]){
	int i, i2, j, deleted_lines = 0;
	char full_line;
	for(i = GRIDY - 1; i >= 0; i--){
		full_line = 1;
		for(j = 0; j < GRIDX; j++){
			if(gameGrid[i][j] != 1){
				full_line = 0;
			}
		}
		if(full_line){
			gameGrid[i][0] = -1;
			for(i2 = i; i2 > 0; i2--){
				for(j = 0; j < GRIDX; j++){
					gameGrid[i2][j] = gameGrid[i2 - 1][j];
				}
			}
			i = GRIDY;
			deleted_lines++;
		}
	}
	return deleted_lines;
}


int rotate(SDL_Rect *rect, int gameGrid[GRIDY][GRIDX], int piece, int rotation){
	int i, x, y;
	int preset[][6] = {
		{  1,  0,  0,  1, -1,  1},
		{  0, -1,  1,  0,  1,  1},
		{  1,  0,  0,  1, -1,  1},
		{  0, -1,  1,  0,  1,  1},

		{ -1,  0,  0,  1,  1,  1},
		{  1, -1,  1,  0,  0,  1},
		{ -1,  0,  0,  1,  1,  1},
		{  1, -1,  1,  0,  0,  1},

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

	if(rotation >= 0)
		for(i = 0; i < 3; i++){
			x = (rect + 0)->x + 
				preset[piece * 4 + rotation][2 * i] * BLOCKSIZE;
			y = (rect + 0)->y + 
				preset[piece * 4 + rotation][2 * i + 1] * BLOCKSIZE;
			
			if((x >= GAMEX) || (x < 0) || (y >= GAMEY))
				return 1;
	
			if(y >= 0)
				if(gameGrid[y / 40][x / 40])
					return 2;
		}
	else
		rotation = 0;

	for(i = 0; i < 3; i++){
		(rect + i + 1 )->x = (rect + 0)->x + 
			preset[piece * 4 + rotation][2 * i] * BLOCKSIZE;

		(rect + i + 1)->y = (rect + 0)->y + 
			preset[piece * 4 + rotation][2 * i + 1] * BLOCKSIZE;
	}
	return 0;
}

void write_text(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color,
		char buffer[], int x, int y){
	SDL_Rect rect;

	SDL_Surface *surface = TTF_RenderText_Solid(font, buffer, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	rect.x = x;
	rect.y = y;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_RenderCopy(renderer, texture, NULL, &rect);

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

	TTF_Init();
	TTF_Font *font = TTF_OpenFont("homevideo.ttf", 48);
	SDL_Color color = {255, 255, 255, 255};
	char buffer[128];

	int gameGrid[GAMEY / BLOCKSIZE][GAMEX / BLOCKSIZE] = {0};
	int lines = 0;

	// initializing rectangles
	SDL_Rect rect[4];
	SDL_Rect next[4];

	for(i = 0; i < 4; i++){
		rect[i].w = BLOCKSIZE; 
		rect[i].h = BLOCKSIZE; 
		next[i].w = BLOCKSIZE; 
		next[i].h = BLOCKSIZE; 
	}

	int piece = rand() % 7, rotation = 0;
	rect[0].x = GAMEX / 2 - BLOCKSIZE;
	rect[0].y = 0;
	rotate(rect, gameGrid, piece, rotation);

	int nextpiece = rand() % 7;
	next[0].x = GAMEX * 3 / 2 - BLOCKSIZE;
	next[0].y = 6 * BLOCKSIZE;
	rotate(next, gameGrid, nextpiece, -1);

	SDL_Rect border;
	border.w = BLOCKSIZE; border.h = GAMEY;
	border.x = GAMEX; border.y = 0;

	char run = 1;
	char fall_help = 0;
	int movePeriodForThisLevel = 40; 
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
						movePeriod = movePeriodForThisLevel / 20;
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
			draw_rectangle(renderer, next + i, 0, 255, 0);
		}
	
		// handling piece move
		if(movePeriodIndex > movePeriod){
			movePeriodIndex = 0;
			if(fall(rect, BLOCKSIZE, gameGrid)){
				if(fall_help > 1){
					// updating grid
					for(i = 0; i < 4; i++){
						gameGrid[(rect + i)->y / BLOCKSIZE]
							[(rect + i)->x / BLOCKSIZE] = 1;
					}
					
					lines += delete_full_lines(gameGrid);


					rect[0].x = GAMEX / 2 - rect[0].w;
					rect[0].y = 0;
					
					piece = nextpiece, rotation = 0;
					rotate(rect, gameGrid, piece, rotation);

					nextpiece = rand() % 7;
					rotate(next, gameGrid, nextpiece, -1);
					fall_help = 0;
					if(gameGrid[0][GRIDX / 2 - 1])
						run = 0;
					if(movePeriodForThisLevel > 1)
						movePeriodForThisLevel = 40 - lines * 3 / 10; 
				}
				fall_help++;
			}
		}
		sprintf(buffer, "Lines-%d", lines);
		write_text(renderer, font, color, buffer, GAMEX+2*BLOCKSIZE, BLOCKSIZE);
		write_text(renderer, font, color, "Next", GAMEX+2*BLOCKSIZE,4*BLOCKSIZE);
		draw_fallen_pieces(renderer, gameGrid);

		SDL_SetRenderDrawColor(renderer, 65, 65, 65, 255);
		SDL_RenderFillRect(renderer, &border);

		SDL_RenderPresent(renderer);

		movePeriodIndex++;
		SDL_Delay(10);
	}
	return 0;
}

