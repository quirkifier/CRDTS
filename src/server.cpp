#define NOMINMAX

#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "command_handler.h"

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET clientSocket, CommandHandler& handler) {
    char buffer[1024];
    std::string inputBuffer;

    std::string welcome = "connected to CRDT KV server\n";
    send(clientSocket, welcome.c_str(), (int)welcome.size(), 0);

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            break;
        }

        inputBuffer.append(buffer, bytesReceived);

        size_t newlinePos;

        while ((newlinePos = inputBuffer.find('\n')) != std::string::npos) {
            std::string command = inputBuffer.substr(0, newlinePos);

            inputBuffer.erase(0, newlinePos + 1);

            while (!command.empty() && (command.back() == '\r' || command.back() == '\n')) {
                command.pop_back();
            }

            if (command.empty()) {
                continue;
            }

            bool shouldQuit = false;
            std::string response = handler.handleCommand(command, shouldQuit);

            response += "\n";

            send(clientSocket, response.c_str(), (int)response.size(), 0);

            if (shouldQuit) {
                closesocket(clientSocket);
                return;
            }
        }
    }

    closesocket(clientSocket);
}

int main() {
    std::string nodeId = "A";
    int port = 6001;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "ERROR WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "ERROR socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "ERROR bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "ERROR listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    KeyValueStore store(nodeId);
    CommandHandler handler(store);

    std::cout << "[Node " << nodeId << "] server started on port " << port << std::endl;
    std::cout << "[Node " << nodeId << "] waiting for clients..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "ERROR accept failed\n";
            continue;
        }

        std::cout << "Client connected\n";

        std::thread clientThread(handleClient, clientSocket, std::ref(handler));
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}