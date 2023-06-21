#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "protocol.h"  
#include "socket_fds.h"
#include "grids.h"
#include "game_grid.c" 


#define PORT 8080
#define MAX_CLIENTS 2
#define SHM_NAME "/my_shm"

int client_count = 0;
int ship_count1 = 5;
int ship_count2 = 5;

void *handle_client(void *arg) {
    int client_index = *((int *)arg);
    free(arg);

    int (grids[2])[GRID_SIZE][GRID_SIZE] = {grid1, grid2};

    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_sockets[client_index], buffer, BUFFER_SIZE);

        printf("Received from client %d: %s\n", client_index, buffer);

        if (strncmp(buffer, MOVE_COMMAND, strlen(MOVE_COMMAND)) == 0) {
            int x, y;
            sscanf(buffer + strlen(MOVE_COMMAND), "%d,%d", &x, &y);

            char* result = check_bomb(grids[client_index], x, y);
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

struct socket_fds create_socket() {
    struct socket_fds socks;
    struct sockaddr_in address;
    struct grids grids;
    int addrlen = sizeof(address);

    if ((socks.server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(socks.server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, 1, sizeof(int))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(socks.server, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(socks.server, 2) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((socks.client1 = accept(socks.server, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    if ((socks.client2 = accept(socks.server, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return socks;
}

int main() {
    struct socket_fds socks;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer1[BUFFER_SIZE] = {0};

    init_grid_with_ships(grid1);
    init_grid_with_ships(grid2);

    printf("Server listening...\n");
    socks = create_socket();

    while (1)
    {
        send(socks.client1, )

        recv(socks.client1, &buffer1, sizeof buffer1, 0);
        printf("Received from client %d: %s\n", client_index, buffer);

    }


    
    
    int *client_index_shared = shared_grid2 + GRID_SIZE * GRID_SIZE;

    while (1) {
        pthread_t thread_id;
        int *client_index_ptr = malloc(sizeof(int));
        *client_index_ptr = client_count; 

        *client_index_shared = client_count;
        msync(client_index_shared, sizeof(int), MS_SYNC);
        printf("Accepted connection from client %d...\n", client_count);

        // send messega from socket
        char client_index_message[BUFFER_SIZE];
        sprintf(client_index_message, "INDEX:%d", client_count);
        send(client_sockets[client_count], client_index_message, strlen(client_index_message), 0);
        
        // Change player
        client_count++;
        if (client_count >= MAX_CLIENTS)
        {
            client_count = 0;
        }

        if (pthread_create(&thread_id, NULL, handle_client, client_index_ptr) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }

    return 0;
}
