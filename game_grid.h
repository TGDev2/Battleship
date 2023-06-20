#ifndef GAME_GRID_H
#define GAME_GRID_H

#define GRID_SIZE 10

void init_grid(int grid[GRID_SIZE][GRID_SIZE]);
int place_ship(int grid[GRID_SIZE][GRID_SIZE], int x, int y, int direction, int size);
void init_grid_with_ships(int grid[GRID_SIZE][GRID_SIZE]);
void display_grid(int grid[GRID_SIZE][GRID_SIZE]);
int record_bomb(int grid[GRID_SIZE][GRID_SIZE], int x, int y);
char* check_bomb(int grid[GRID_SIZE][GRID_SIZE], int x, int y);

#endif
