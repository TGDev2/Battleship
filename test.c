#include <stdio.h>
#include "game_grid.c"

int main() {
    // Initialiser la grille
    int grid[GRID_SIZE][GRID_SIZE];
    init_grid_with_ships(grid);

    // Afficher la grille initiale
    printf("Grille initiale:\n");
    display_grid(grid);

    // Lancer une bombe à une position spécifique
    int x = 3;
    int y 1 = 4;

    x = x - 1;
    y = y - 1;
    
    printf("\nCheck Bomb à la position (%d, %d) : %s\n", x, y, check_bomb(grid, x, y));

    // Afficher la grille après le lancement de la bombe
    printf("\nGrille après le lancement de la bombe:\n");
    display_grid(grid);

    return 0;
}