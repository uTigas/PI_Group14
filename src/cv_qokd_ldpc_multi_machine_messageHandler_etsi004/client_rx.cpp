#ifdef WIN32
#define OperatingSystem 0
#include <WinSock2.h>
#include <unistd.h>
//#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib
WSADATA wsaData;
if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "Erro ao inicializar o Winsock." << std::endl;
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
#define PORT 51001  // Server port

#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>

int main() {
    char buffer[1024];
    int file_counter = 0, bytesRead;
    bool end_of_key_exchange = false, first_line = true;
    std::string more_files = "true";
    std::string ask_key_message = "key";
    std::string good = "Received";
    std::string msg(good.c_str(), good.size());
    std::ostringstream filename, renamed_filename;
    
    std::cout << "###############################################################################################" << std::endl;
    std::cout << "Client RX" << std::endl;
    std::cout << "###############################################################################################" << std::endl;


    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error trying to create the socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT); // Server port
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS); // Server IP address

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error trying to connect the server. Trying again..." << std::endl;
        do {
            if(connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) >= 0) {
                break;
            }
        } while (true);
    }

    std::cout << "Client connected to Server IP " << IP_ADDRESS << ":" << PORT << std::endl;


    do {
        do {
            memset(buffer, 0, sizeof(buffer));
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead == -1) {
                std::cerr << "Error trying to receive data from the server." << std::endl;
                close(clientSocket);
                return 1;
            }
        } while (bytesRead <= 0);
        ask_key_message = "";
        //std::cout << "Received from server: " << buffer << std::endl;
        if(std::string(buffer) == "ready") {
            ask_key_message = "key";
            send(clientSocket, ask_key_message.c_str(), ask_key_message.size(), 0);
            //std::cout << "Sent to server: " << ask_key_message << std::endl;
        }

        if(OperatingSystem == 0) { // Windows
            memset(buffer, 0, sizeof(buffer));
            recv(clientSocket, buffer, sizeof(buffer), 0);
            std::cout << buffer << std::endl;
            break;
        } else if(OperatingSystem == 1) { // Linux
            filename.str("");
            filename << "rx_raw_generating" << file_counter << ".dat";;
            std::cout << "File: " << filename.str() << std::endl;
            std::ofstream file(filename.str());
            end_of_key_exchange = false;
            first_line = true;
            if (!file.is_open()) std::cerr << "Failed to open file: " << filename.str() << std::endl;
            while(!end_of_key_exchange) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                //std::cout << "Buffer: " << buffer << std::endl;
                if(std::string(buffer) == "end_of_file" || bytesRead <= 0) {
                    end_of_key_exchange = true;
                    file.close();
                } else {
                    send(clientSocket, msg.c_str(), msg.size(), 0);
                    if(first_line) {
                        file << buffer;
                        first_line = false;
                    }
                    file << std::endl << buffer;
                }
            }
        } else {
            std::cerr << "Operating system not supported." << std::endl;
            return 1;
        }
        renamed_filename.str("");
        renamed_filename << "rx_raw_new" << file_counter << ".dat";
        rename(filename.str().c_str(),renamed_filename.str().c_str());
        file_counter++;
    } while (ask_key_message != "exit");

    // Close the socket
    close(clientSocket);

    return 0;
}