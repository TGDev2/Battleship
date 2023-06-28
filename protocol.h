#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT 8080
#define MAX_CLIENTS 2

// Taille maximale du buffer de la commande
#define BUFFER_SIZE 1024

// Commande pour déplacer le navire
#define MOVE_COMMAND "MOVE:"
// Commande pour signaler le résultat d'un tir
#define OUTCOME_COMMAND "OUTCOME:"

// Les résultats possibles d'un tir
#define HIT "HIT"    // Le tir a touché un navire
#define MISS "MISS"  // Le tir a raté tous les navires
#define SUNK "SUNK"  // Le tir a coulé un navire

// Nombre total de navires dans le jeu
#define NUM_SHIPS 5
// Taille des navires. {5, 4, 3, 3, 2} correspond à un porte-avions, un cuirassé, un croiseur, un sous-marin, et un destroyer
#define SHIP_SIZES {5, 4, 3, 3, 2}

// Taille maximale des navires (ici, c'est le porte-avions avec une taille de 5)
#define MAX_SHIP_SIZE 5

#define GRID_SIZE 10
#define MESSAGE_SIZE 50

#define EMPTY_CELL 0
#define SHIP_CELL 1
#define HIT_CELL 2
#define MISS_CELL 3

#endif
