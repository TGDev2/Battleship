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
#include "message.h"
#include "array.h"
#include "game_grid.c" 
#include "game_engine.c" 

struct socket_fds create_socket() {
    struct socket_fds socks;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((socks.server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
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
    
    printf("Accepted connection from client 1\n");

    if ((socks.client2 = accept(socks.server, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Accepted connection from client 2\n");

    return socks;
}

void manage_client(struct socket_fds socks, struct grids grids, int number, int ship_count1, int ship_count2) {
    char buffer[BUFFER_SIZE] = {0};
    struct message msg;
    int actual_sock;

    // define sock number from player number
    if (number == 1) {
        actual_sock = socks.client1;
    }
    else {
        actual_sock = socks.client2;
    }

    // create message
    copyarray(msg.grid_client1, grids.grid_client1);
    copyarray(msg.grid_client2, grids.grid_client2);
    strncpy(msg.message, "Serveur envoie les grilles", sizeof(msg.message));

    send(actual_sock, &msg, BUFFER_SIZE, 0);
    memset(&msg, 0, sizeof(msg));

    read(actual_sock, &buffer, sizeof buffer);
    printf("Received from client %d: %s\n", number, buffer);

    int x, y;
    sscanf(buffer, "MOVE:%d,%d", &x, &y);

    memset(&buffer, 0, sizeof buffer);

    // Pour client 1 check bombe adversaire / pour client 2 check bombe adversaire
    char* result;
    if (number == 1) {
        result = check_bomb(grids.grid_client2, x, y);
    }
    else {
        result = check_bomb(grids.grid_client1, x, y);
    }

    if (is_end(socks, grids, ship_count1, ship_count2, result, number) == 1) {
        return;
    }

    // create message
    copyarray(msg.grid_client1, grids.grid_client1);
    copyarray(msg.grid_client2, grids.grid_client2);
    sprintf(msg.message, "%s%s", OUTCOME_COMMAND, result);

    send(actual_sock, &msg, BUFFER_SIZE, 0);
    memset(&msg, 0, sizeof(msg));
}

int main() {
    struct socket_fds socks;
    struct grids grids;
    int ship_count1 = 5;
    int ship_count2 = 5;

    init_grid_with_ships(grids.grid_client1);
    init_grid_with_ships(grids.grid_client2);

    printf("Server listening...\n");
    socks = create_socket();

    while (1)
    {
        // Client 1
        manage_client(socks, grids, 1, ship_count1, ship_count2);

        // Client 2
        manage_client(socks, grids, 2, ship_count1, ship_count2);
    }

    return 0;
}
