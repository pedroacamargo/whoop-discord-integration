#include <thread>
#include <iostream>
#include "discord_listener.h"
#include "webhook_server.h"

int main() {
    std::thread discord_thread(startDiscordClient);

    std::cout << "Discord client is running..." << std::endl;

    std::thread webhook_thread([]() {
        webhook::TcpServer server = webhook::TcpServer("0.0.0.0", 8080);

        std::cout << "Webhook server is running..." << std::endl;
    });

    
    std::cout << "Joining Webhook thread...\n";
    webhook_thread.join();
    std::cout << "Webhook thread joined.\n";
    
    std::cout << "Joining Discord thread...\n";
    discord_thread.join();
    std::cout << "Discord thread joined.\n";
    
    std::cout << "Both Discord client and webhook server have stopped." << std::endl;

    return 0;
}