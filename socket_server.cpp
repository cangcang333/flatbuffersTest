//
// Created by Danial on 2020/8/27.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "monster_generated.h"
using namespace MyGame::Sample;

#define LISTEN_BACKLOG 50

#define SERV_PORT 9876

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char *argv[])
{
    int sfd, cfd;
    struct sockaddr_in server_addr, peer_addr;
    socklen_t peer_addr_size;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        handle_error("socket");
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));  /* Clear structure */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    if (bind(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        handle_error("bind");
    }

    if (listen(sfd, LISTEN_BACKLOG) == -1)
    {
        handle_error("listen");
    }

    /*
     * I don't know the incoming data length,
     * This need to be optimized, find a better way
     * to handle it.
     */
    uint8_t buffer_pointer[1000];
    /* Now we can accept incoming connections one at a time using accept(2) */
    while (true)
    {
        peer_addr_size = sizeof(peer_addr);
        cfd = accept(sfd, (struct sockaddr * ) &peer_addr, &peer_addr_size);
        if (cfd == -1)
        {
            handle_error("accept");
        }
        printf("client file descriptor is %d\n", cfd);


        //int byte_read = read(cfd, (void *) &server_orc, sizeof(server_orc));
        int byte_recv = recv(cfd, buffer_pointer, sizeof(buffer_pointer), 0);
        printf("server read orc of %d bytes\n", byte_recv);

        /*
        // Read directly from monster binary file
        std::ifstream ifs;
        ifs.open("orc.bin", std::ios::binary | std::ios::in);
        char *buffer_pointer;
        buffer_pointer = (char *) malloc(sizeof(char) * 1000);
        ifs.read(buffer_pointer, 1000);
        */

        auto monster = GetMonster(buffer_pointer);
        std::cout << monster->hp() << std::endl;
        std::cout << monster->mana() << std::endl;

        std::cout << monster->name()->c_str() << std::endl;
        std::cout << monster->color() << std::endl;
        std::cout << monster->path() << std::endl;

        close(cfd);

    }

    return 0;
}

