#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "protocol.h"  
#include "game_grid.h" 
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define SHM_NAME "/my_shm"

int client_sockets[MAX_CLIENTS] = {0};
int client_count = 0;
int ship_count1 = 5;
int ship_count2 = 5;

int grid1[GRID_SIZE][GRID_SIZE];
int grid2[GRID_SIZE][GRID_SIZE];

void *handle_client(void *arg) {
    int client_index = *((int *)arg);
    free(arg);

    int (*grids[2])[GRID_SIZE][GRID_SIZE] = {&grid1, &grid2};

    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_sockets[client_index], buffer, BUFFER_SIZE);

        printf("Received from client %d: %s\n", client_index, buffer);

        if (strncmp(buffer, MOVE_COMMAND, strlen(MOVE_COMMAND)) == 0) {
            int x, y;
            sscanf(buffer + strlen(MOVE_COMMAND), "%d,%d", &x, &y);

            char* result = check_bomb(*grids[client_index], x, y);
            char response_message[BUFFER_SIZE * 2];
            sprintf(response_message, "%s%s", OUTCOME_COMMAND, result);
            write(client_sockets[client_index], response_message, strlen(response_message));

            if (strcmp(result, HIT) == 0 || strcmp(result, SUNK) == 0) {
                if (client_index == 0) ship_count2--;
                else ship_count1--;
        
                if (ship_count1 == 0) {
                    char end_message[] = "END: Player 2 wins!";
                    write(client_sockets[0], end_message, strlen(end_message));
                    write(client_sockets[1], end_message, strlen(end_message));
                    exit(0); 
                } else if (ship_count2 == 0) {
                    char end_message[] = "END: Player 1 wins!";
                    write(client_sockets[0], end_message, strlen(end_message));
                    write(client_sockets[1], end_message, strlen(end_message));
                    exit(0);  
                }
            }
        }
    }
}

int main() {
    int shm_fd;
    int *shared_grid1;
    int *shared_grid2;
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, 2 * GRID_SIZE * GRID_SIZE * sizeof(int) + sizeof(int));

    shared_grid1 = mmap(0, GRID_SIZE * GRID_SIZE * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    shared_grid2 = shared_grid1 + GRID_SIZE * GRID_SIZE;

    init_grid_with_ships((int (*)[GRID_SIZE])shared_grid1);
    init_grid_with_ships((int (*)[GRID_SIZE])shared_grid2);

    memcpy(grid1, shared_grid1, GRID_SIZE * GRID_SIZE * sizeof(int));
    memcpy(grid2, shared_grid2, GRID_SIZE * GRID_SIZE * sizeof(int));

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    int *client_index_shared = shared_grid2 + GRID_SIZE * GRID_SIZE;
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening...\n");

    while (1) {
        pthread_t thread_id;
        int *client_index_ptr = malloc(sizeof(int));
        *client_index_ptr = client_count; 
        if ((client_sockets[client_count] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        *client_index_shared = client_count;
        msync(client_index_shared, sizeof(int), MS_SYNC);
        printf("Accepted connection from client %d...\n", client_count);
        char client_index_message[BUFFER_SIZE];
        sprintf(client_index_message, "INDEX:%d", client_count);
        send(client_sockets[client_count], client_index_message, strlen(client_index_message), 0);
        client_count++;
        if (client_count >= MAX_CLIENTS) {
            break;
        }

        if (pthread_create(&thread_id, NULL, handle_client, client_index_ptr) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }
    munmap(shared_grid1, 2 * GRID_SIZE * GRID_SIZE * sizeof(int));

    shm_unlink(SHM_NAME);
    return 0;
}
