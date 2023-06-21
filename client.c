#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "protocol.h"
#include "grids.h"
#include "array.h"
#include "message.h"
#include "game_grid.c"

int connect_socket() {
    int sock;
    struct sockaddr_in serv_addr;

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

    return sock;
}

int client(int number) {
    char buffer[BUFFER_SIZE] = {0};
    struct grids grids;
    struct message msg_recv;

    int sock = connect_socket();

    int grid[GRID_SIZE][GRID_SIZE];

    while (1) {
        // recevied and read message from server
        read(sock, &buffer, sizeof(buffer));
        memcpy(&msg_recv, buffer, sizeof msg_recv);

        copyarray(grids.grid_client1, msg_recv.grid_client1);
        copyarray(grids.grid_client2, msg_recv.grid_client2);
        printf("Outcome: %s\n", msg_recv.message);

        memset(&buffer, 0, sizeof(buffer));
        memset(&msg_recv, 0, sizeof(msg_recv));

        printf("Enter your move: ");

        char pos_str[4];
        fgets(pos_str, 4, stdin);
        char move_message[MESSAGE_SIZE];
        sprintf(move_message, "%s%s", MOVE_COMMAND, pos_str);
        send(sock, move_message, strlen(move_message), 0);

        // recevied and read message from server
        read(sock, &buffer, sizeof(buffer));
        memcpy(&msg_recv, buffer, sizeof msg_recv);

        copyarray(grids.grid_client1, msg_recv.grid_client1);
        copyarray(grids.grid_client2, msg_recv.grid_client2);

        printf("\nCurrent Grid:\n");
        if (number == 1) {
            display_grid(grids.grid_client1);
        }
        else {
            display_grid(grids.grid_client2);
        }
        printf("Outcome: %s\n", msg_recv.message);

        if (strncmp(buffer, "END", 3) == 0) {
            printf("%s\n", buffer + 4);
            exit(0);
        }

        memset(&buffer, 0, sizeof(buffer));
        memset(&msg_recv, 0, sizeof(msg_recv));
    }
    return 0;
}