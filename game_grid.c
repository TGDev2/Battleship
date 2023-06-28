#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "protocol.h"

void init_grid(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
}

int place_ship(int grid[GRID_SIZE][GRID_SIZE], int x, int y, int direction, int size) {
    if(direction == 0 && y + size > GRID_SIZE) {
        return 0;
    }
    else if(direction == 1 && x + size > GRID_SIZE) {
        return 0;
    }

    for (int i = 0; i < size; i++) {
        if (direction == 0) {
            if (grid[x][y+i] != 0) {
                return 0;
            }
        } else {
            if (grid[x+i][y] != 0) {
                return 0;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        if (direction == 0) {
            grid[x][y+i] = 1;
        } else {
            grid[x+i][y] = 1;
        }
    }
    return 1;
}

void init_grid_with_ships(int grid[GRID_SIZE][GRID_SIZE]) {
    int ship_sizes[] = SHIP_SIZES;
    init_grid(grid);
    for (int i = 0; i < NUM_SHIPS; i++) {
        int ship_placed = 0;
        while (!ship_placed) {
            int x = rand() % (GRID_SIZE - ship_sizes[i] + 1);
            int y = rand() % (GRID_SIZE - ship_sizes[i] + 1);
            int direction = rand() % 2;
            ship_placed = place_ship(grid, x, y, direction, ship_sizes[i]);
        }
    }
}


void display_grid(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

char* check_bomb(int grid[GRID_SIZE][GRID_SIZE], int x, int y) {
    if (grid[x][y] == 1) {  // Si la bombe a touché un navire
        grid[x][y] = 2;  // Marquer la case comme bombardée
        for (int i = -MAX_SHIP_SIZE; i <= MAX_SHIP_SIZE; i++) {
            if (x + i >= 0 && x + i < GRID_SIZE) {
                if (grid[x + i][y] == 1) {
                    return "HIT";
                }
            }
            if (y + i >= 0 && y + i < GRID_SIZE) {
                if (grid[x][y + i] == 1) {
                    return "HIT";
                }
            }
        }
        return "SUNK";
    } else if (grid[x][y] == 0) {  // Si la bombe a touché une case vide
        grid[x][y] = 3;  // Marquer la case comme bombardée
        return "MISS";
    } else {
        return "ERROR";
    }
}
