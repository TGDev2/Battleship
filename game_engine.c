#pragma once
#include "protocol.h"
#include "message.h"
#include "socket_fds.h"

int is_end(struct socket_fds socks, struct grids grids, int ship_count1, int ship_count2, char *result, int actual_client) {
    if (strcmp(result, SUNK) == 0)
    {
        if (actual_client == 1)
            ship_count1--;
        else
            ship_count2--;

        if (ship_count1 == 0 || ship_count2 == 0) {
            struct message msg;
            copyarray(msg.grid_client1, grids.grid_client1);
            copyarray(msg.grid_client2, grids.grid_client2);
            sprintf(msg.message, "END: Player %d wins!", 3 - actual_client);

            send(socks.client1, &msg, sizeof(msg), 0);
            send(socks.client2, &msg, sizeof(msg), 0);
            return 1;
        }
    }
    return 0;
}
