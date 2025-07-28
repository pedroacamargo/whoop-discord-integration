#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>

namespace webhook {
    class TcpServer {
        public:
            TcpServer(std::string ip, int port = 8080);
            ~TcpServer();
        private:
            std::string ip_address;
            int port;
            int tcpSocket;
            int new_socket;
            long incomingMessage;
            struct sockaddr_in address;
            unsigned int socketAddressLength;
            std::string serverMessage;

            int startServer();
            void closeServer();
        };
}