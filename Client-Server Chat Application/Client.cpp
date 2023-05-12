#include <iostream>
#include <cstring>
#include <WinSock2.h>
#include <string>
#include <WS2tcpip.h>

// Server IP address and port number
const char* SERVER_IP = "127.0.0.1";  // Replace with the actual server IP
const int SERVER_PORT = 12345;        // Replace with the actual server port

// Server response codes
const int SV_FULL = 0;
const int SV_SUCCESS = 1;

int main() {
    // Step 1: Prompt the user for the server's IP address and port number
    // This step will be replaced by the information received on the UDP connection in phase 2

    // Step 2: Create a TCP socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Step 3: Connect to the server
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        closesocket(clientSocket);
        return 1;
    }

    if (connect(clientSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        return 1;
    }

    // Step 4: Prompt for a username
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // Step 5: Call the command $register with the username
    std::string registerCommand = "$register " + username;
    if (send(clientSocket, registerCommand.c_str(), registerCommand.length(), 0) == -1) {
        std::cerr << "Error sending registration command" << std::endl;
        closesocket(clientSocket);
        return 1;
    }

    // Step 6: Receive the server response
    int response;
    if (recv(clientSocket, reinterpret_cast<char*>(&response), sizeof(response), 0) == -1) {
        std::cerr << "Error receiving server response" << std::endl;
        closesocket(clientSocket);
        return 1;
    }


    if (response == SV_FULL) {
        std::cout << "Server is full. Registration failed." << std::endl;
        closesocket(clientSocket);
        return 0;
    }
    else if (response == SV_SUCCESS) {
        std::cout << "Registration successful. You can now send commands and chat messages." << std::endl;
    }
    else {
        std::cerr << "Unknown server response" << std::endl;
        closesocket(clientSocket);
        return 1;
    }

    // Step 7: Client can execute commands and send chat messages
    // Implement this functionality in Phase 2

    // Close the socket when done
    closesocket(clientSocket);

    return 0;
}
