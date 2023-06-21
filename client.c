#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "game_grid.c"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SHM_NAME "/my_shm"
#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    int *shared_grid;
    int shm_fd;

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

    shared_grid = mmap(0, 2 * GRID_SIZE * GRID_SIZE * sizeof(int), PROT_READ, MAP_SHARED, shm_fd, 0);

    int *client_index_shared = shared_grid + 2 * GRID_SIZE * GRID_SIZE;
    int client_index = *client_index_shared;

    if (client_index == 0) {
        shared_grid += GRID_SIZE * GRID_SIZE;  
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    int grid[GRID_SIZE][GRID_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        
        if (strncmp(buffer, OUTCOME_COMMAND, strlen(OUTCOME_COMMAND)) == 0) {
            printf("Outcome: %s\n", buffer + strlen(OUTCOME_COMMAND));
            memcpy(grid, shared_grid, GRID_SIZE * GRID_SIZE * sizeof(int));
            printf("\nCurrent Grid:\n");
            display_grid((int (*)[GRID_SIZE])grid);
        } 
        
        if (strncmp(buffer, "END", 3) == 0) {
            printf("%s\n", buffer + 4);
            exit(0);
        }

        printf("Enter your move: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        char move_message[BUFFER_SIZE];
        sprintf(move_message, "%s%s", MOVE_COMMAND, buffer);
        send(sock, move_message, strlen(move_message), 0);
    }
    return 0;
}
