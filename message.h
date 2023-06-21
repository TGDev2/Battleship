#ifndef MESSAGE_H
#define MESSAGE_H

#include "protocol.h"

struct message {
    int grid_client1[GRID_SIZE][GRID_SIZE];
    int grid_client2[GRID_SIZE][GRID_SIZE];
    char message[MESSAGE_SIZE];
};

#endif