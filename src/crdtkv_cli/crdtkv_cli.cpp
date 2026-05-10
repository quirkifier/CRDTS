#define NOMINMAX

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::string receiveResponse(SOCKET socket) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        return "";
    }

    return std::string(buffer, bytesReceived);
}

int main() {
    std::string host = "127.0.0.1";
    int port = 6001;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "ERROR WSAStartup failed\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "ERROR socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << "ERROR invalid address\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "ERROR could not connect to server\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "connected to CRDT KV server at " << host << ":" << port << "\n";

    // Read welcome message from server immediately.
    std::string welcome = receiveResponse(clientSocket);
    if (!welcome.empty()) {
        std::cout << welcome;
    }

    std::cout << "Type commands. Example:\n";
    std::cout << "CREATE likes GCOUNTER\n";
    std::cout << "INC likes 5\n";
    std::cout << "GET likes\n";
    std::cout << "QUIT\n\n";

    while (true) {
        std::string command;

        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.empty()) {
            continue;
        }

        std::string commandToSend = command + "\n";

        int sentBytes = send(clientSocket, commandToSend.c_str(), (int)commandToSend.size(), 0);

        if (sentBytes == SOCKET_ERROR) {
            std::cout << "ERROR failed to send command\n";
            break;
        }

        std::string response = receiveResponse(clientSocket);

        if (response.empty()) {
            std::cout << "Server disconnected\n";
            break;
        }

        std::cout << response;

        if (command == "QUIT" || command == "quit") {
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}