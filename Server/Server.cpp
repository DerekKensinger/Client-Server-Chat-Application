#include <iostream>
#include <string>
#include <cstring>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

// Server IP address and port number
const char* SERVER_IP = "127.0.0.1";  // Replace with the actual server IP
const int SERVER_PORT = 12345;        // Replace with the actual server port

// Server response codes
const int SV_FULL = 0;
const int SV_SUCCESS = 1;

// Maximum number of connected clients
const int MAX_CLIENTS = 10;

// Structure to store client information
struct ClientInfo {
    std::string username;
    SOCKET socket;
};

// Function to send a message to a specific client
void sendMessageToClient(SOCKET clientSocket, const std::string& message) {
    send(clientSocket, message.c_str(), message.length(), 0);
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    // Step 1: Create the TCP listening socket
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create listening socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 2: Bind the socket
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(listeningSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind listening socket" << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    // Step 3: Set the server in listening mode
    if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error listening on socket" << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    // Step 4: Create the file descriptor sets
    fd_set masterSet, readySet;
    FD_ZERO(&masterSet);
    FD_SET(listeningSocket, &masterSet);
    SOCKET maxSocket = listeningSocket;

    // Step 5: Call select and return after timeout
    timeval timeout{};
    timeout.tv_sec = 1;  // Set the timeout to 1 second

    while (true) {
        readySet = masterSet;  // Reset the ready set

        int rc = select(0, &readySet, NULL, NULL, &timeout);
        if (rc == SOCKET_ERROR) {
            std::cerr << "Error in select" << std::endl;
            break;
        }

        if (rc == 0) {
            // Timeout occurred, no socket is ready
            continue;
        }

        // Check if the listening socket is set in the ready set
        if (FD_ISSET(listeningSocket, &readySet)) {
            // Accept the connection
            sockaddr_in clientAddress{};
            int clientAddressSize = sizeof(clientAddress);
            SOCKET clientSocket = accept(listeningSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
               
                if (clientSocket == INVALID_SOCKET) {
                    std::cerr << "Failed to accept client connection" << std::endl;
                    continue;
                }

            // Add the client socket to the master set
            FD_SET(clientSocket, &masterSet);

            // Update the maximum socket descriptor
            if (clientSocket > maxSocket) {
                maxSocket = clientSocket;
            }

            // Print the client's IP and port
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
            std::cout << "New client connected: " << clientIP << ":" << ntohs(clientAddress.sin_port) << std::endl;
        }

        // Run through the ready set and receive data from the client sockets
        for (SOCKET clientSocket = 0; clientSocket <= maxSocket; ++clientSocket) {
            if (FD_ISSET(clientSocket, &readySet)) {
                if (clientSocket == listeningSocket) {
                    // Skip the listening socket
                    continue;
                }

                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));

                // Receive data from the client
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    // Error or connection closed by client
                    if (bytesRead == 0) {
                        // Connection closed
                        std::cout << "Client disconnected" << std::endl;
                    }
                    else {
                        std::cerr << "Error receiving data from client" << std::endl;
                    }

                    // Close the client socket and remove it from the master set
                    closesocket(clientSocket);
                    FD_CLR(clientSocket, &masterSet);
                }
                else {
                    // Process the received message
                    std::string message(buffer);

                    // Handle the $register command
                    if (message.substr(0, 9) == "$register") {
                        // Extract the username from the message
                        std::string username = message.substr(10);

                        // TODO: Check the chat capacity

                        // Send SV_FULL if chat is full
                        // TODO: Implement capacity check and send SV_FULL if necessary

                        // Save the username and acknowledge registration completion
                        // TODO: Implement saving the username and send SV_SUCCESS
                    }

                    // Handle the $getlist command
                    else if (message == "$getlist") {
                        // TODO: Send the list of connected users as a comma-separated string
                    }

                    // Handle the $getlog command
                    else if (message == "$getlog") {
                        // TODO: Open the log file on the server

                        // TODO: Send the length of the file first to tell the client how much data to receive

                        // TODO: Read each line from the log file and send it to the client

                        // TODO: Close the log file
                    }

                    // Handle the $exit command
                    else if (message == "$exit") {
                        // TODO: Handle the user exit, remove the user from the list and close the connection
                    }

                    // Unknown command
                    else {
                        std::cerr << "Unknown command received" << std::endl;
                    }
                }
            }
        }
    }

    // Close the listening socket
    closesocket(listeningSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
