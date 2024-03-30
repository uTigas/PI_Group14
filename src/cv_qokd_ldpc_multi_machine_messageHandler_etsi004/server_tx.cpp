#ifdef WIN32
#define OperatingSystem 0
#include <WinSock2.h>
#include <unistd.h>
#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

WSADATA wsaData;
if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "Error trying to initialize Winsock." << std::endl;
    return 1;
}

#else

#define OperatingSystem 1
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#endif

// IP address and Port
#define IP_ADDRESS "127.0.0.1"
#define PORT 51000  // Server listening port

#include <iostream>
#include <algorithm>
#include <cstring>

#include <fstream>
#include <sstream>
#include <iomanip>

int main() {
    int file_counter = 0;
    char buffer[1024];
    ssize_t bytesRead, bytesSent;
    memset(buffer, 0, sizeof(buffer));
    std::string file_line = "";
    std::string message, response, text_to_send, next_file_status;
    std::ostringstream filename, next_file;
    
    std::cout << "###############################################################################################" << std::endl;
    std::cout << "Server TX" << std::endl;
    std::cout << "###############################################################################################" << std::endl;

 
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error trying to create the socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error trying to associate the socket with the server address." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error trying to set the socket to listen mode." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server started with IP " << IP_ADDRESS << ":" << PORT << std::endl;
    std::cout << "Waiting for connections..." << std::endl;

    while (true) {
        // New connection
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Error trying to accept the connection." << std::endl;
            close(serverSocket);
            return 1;
        } else {
            std::cout << "Client with IP " << inet_ntoa(clientAddress.sin_addr) << ":" << clientAddress.sin_port << " is connected to the server" << std::endl;
       }

        do {
            next_file.str("");
            next_file << "tx_raw" << file_counter << ".dat";
            std::ifstream next_file_(next_file.str());
            do {
                std::string next_file_available = next_file_.is_open() ? "ready" : "not_ready";
                next_file_status = std::string(next_file_available.c_str(), next_file_available.size());
                //std::cout << "Status: " << next_file_status << std::endl;
            } while(next_file_status != "ready");
            send(clientSocket, next_file_status.c_str(), next_file_status.size(), 0);
            memset(buffer, 0, sizeof(buffer));
            
            
            recv(clientSocket, buffer, sizeof(buffer), 0);
            next_file_.close();
            //std::cout << "Buffer from client: " << std::string(buffer) << std::endl;

            std::string receivedData = std::string(buffer);
            std::transform(receivedData.begin(), receivedData.end(), receivedData.begin(), ::tolower);
            if(receivedData == "exit"){
                break;
            } else {
                if(receivedData == "key") {
                    filename.str("");
                    filename << "tx_raw" << file_counter << ".dat";
                    std::ifstream file(filename.str());
                    if (!file.is_open()) {
                        std::cerr << "Failed to open file: " << filename.str() << std::endl;
                    } else {
                        file_line = "";
                        std::cout << "File: " << filename.str() << std::endl;
                        while (std::getline(file, file_line)) {
                            //std::cout << file_line << std::endl;
                            send(clientSocket, file_line.c_str(), file_line.size(), 0);
                            memset(buffer, 0, sizeof(buffer));
                            recv(clientSocket, buffer, sizeof(buffer), 0);
                        }
                        file_line = "end_of_file";
                        send(clientSocket, file_line.c_str(), file_line.size(), 0);
                        file.close();
                    }
                    file_counter++;
                } else {

                    if (bytesSent == -1) {
                        std::cerr << "Error trying to send a response to the client." << std::endl;
                        close(clientSocket);
                        close(serverSocket);
                        return 1;
                    }
                }
            }

        } while (message != "exit");

        close(clientSocket);
        break;

    }

    // Close the socket
    close(serverSocket);

    return 0;
}
