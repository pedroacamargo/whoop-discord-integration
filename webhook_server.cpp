#include "webhook_server.h"
#include <iostream>
#include <unistd.h>
#include <sstream>

namespace logger {
    void log(const std::string& message) {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string& message) {
        log("Error: " + message);
        exit(EXIT_FAILURE);
    }
}


namespace webhook {
    TcpServer::TcpServer(std::string ip, int port) : ip_address(ip), port(port), tcpSocket(), new_socket(), incomingMessage(), socketAddressLength(sizeof(address)), serverMessage("Hello from TCP Server"), address() {
        startServer();
    }

    TcpServer::~TcpServer() {
        closeServer();
        std::cout << "TCP Server destroyed." << std::endl;
    }

    int TcpServer::startServer() {
        tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (tcpSocket < 0) {
            logger::exitWithError("Socket creation failed");
            return -1;
        }

        logger::log("Socket created successfully");

        return 0;
    }

    void TcpServer::closeServer() {
        if (tcpSocket != -1) {
            close(tcpSocket);
            close(new_socket);
            tcpSocket = -1;
            new_socket = -1;
            std::cout << "TCP Server closed." << std::endl;
            exit(0);
        }
    }
}