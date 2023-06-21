#include "protocol.h"

#ifndef ARRAY_H
#define ARRAY_H

/// @brief Coppy 2D array with value
/// @param dest Destinatary 2D array
/// @param src Source 2D array
int copyarray(int dest[GRID_SIZE][GRID_SIZE], const int src[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
    return 1;
}

#endif
