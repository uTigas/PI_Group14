//# include "ip_tunnel_20200819.h" 
# include "ip_tunnel_linux_20200819.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

// ############################################################
//  these constants are useful, but appear to be specific to */
//  winsock; so, I define them here and use them as if they */
//  were standard */
#define INVALID_SOCKET ((~0))
#define SOCKET_ERROR (-1)
// ############################################################

void IPTunnel::initialize(void)
{
	// Initialize the Server 
	// The server is assumed to be the block without input signals,
	// i.e. the block that is going to receive the data.
	if (inputSignals.empty()) {
		if (transportProtocol == transport_protocol::TCP) 
		{
			if (getVerboseMode())
			{
				std::cout << "TCP Server" << std::endl;
			}

			if (!server_tcp())
			{
				std::cerr << "Error opening TCP server\n";
				exit(1);
			}
		}
		else {
			if (getVerboseMode())
			{
				std::cout << "UDP Server" << std::endl;
			}
			if (!server_udp())
			{
				std::cerr << "Error opening UDP server\n";
				exit(1);
			}
		}
		// Resize the local buffer
		if (outputSignals[0]->getValueType() == (signal_value_type::t_message))
			ip_tunnel_buffer.resize(outputSignals[0]->getBufferLength()*MAX_MESSAGE_SIZE);
		else
			ip_tunnel_buffer.resize(outputSignals[0]->getBufferLength()*outputSignals[0]->valueTypeSizeOf() / sizeof(char));
	}




	// Initialize the Client
	// The client is assumed to be the block without output signals,
	// i.e. the block that is going to transmit the data.
	else { 
		if (transportProtocol == transport_protocol::TCP) 
		{
			if (getVerboseMode())
			{
				std::cout << "TCP Client" << std::endl;
			}
			if (!client_tcp())
			{
				std::cerr << "Error opening client" << std::endl;
				exit(1);
			}
		}
		else {
			std::cerr << "UDP Client" << std::endl;
			if (!client_udp()) std::cerr << "Error opening client\n";
		}

		// Resize the local buffer
		if (inputSignals[0]->getValueType()==(signal_value_type::t_message))
			ip_tunnel_buffer.resize(inputSignals[0]->getBufferLength()*MAX_MESSAGE_SIZE);
		else
			ip_tunnel_buffer.resize(inputSignals[0]->getBufferLength()*inputSignals[0]->valueTypeSizeOf() / sizeof(char));	
	}
}


bool IPTunnel::runBlock(void)
{
	t_bool alive{ true };
	size_t process{0};

	// Run Server/Receiver
	if (inputSignals.empty())
	{ 
		fd_set fread, fwrite, fexception;  // array of socket descriptors. Structure to be used with the select() method 
		FD_ZERO(&fread);
		FD_ZERO(&fwrite);
		FD_ZERO(&fexception);
		
		if (!connectionEstablished)
		{
			FD_SET(serverSocket[0], &fread); // in a listening socket only the fread and fexception are used
			FD_SET(serverSocket[0], &fexception);

			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 1;
			int nfds{ 0 }; //Ignored. The nfds parameter is included only for compatibility with Berkeley sock 
			auto s = select(nfds, &fread, &fwrite, &fexception, &tv); // pull to check if there is any connection request
			if (s > 0)
			{
				sockaddr_in client;
				int clientSize = sizeof(client);
				int s = accept(serverSocket[0], (sockaddr*)&client, (socklen_t*)(&clientSize));
				if (s == INVALID_SOCKET)
				{
					std::cerr << "accept failed with error no. : " << errno << std::endl;

					exit( 1 );
				}
				else
				{
					serverSocket.push_back(s);
					connectionEstablished = true;
					if (getVerboseMode())
					{
						char ipAddress[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &client.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
						std::cout << "Client " << remoteMachineIpAddress << " connect throught port " << localMachinePort << "!" << std::endl;
					}
				}
			}
			else
			{
				if (getVerboseMode()) std::cout << "No connection request received." << std::endl;
				return true;
			}
		}
		else
		{

		size_t space = outputSignals[0]->space();
		for (auto k : outputSignals)
            		space = std::min((int)space, (int)k->space());
		
		if (space == 0) return false;

		//----------------------------------------RECEIVING THE SIGNAL----------------------------------------

		if (transportProtocol == transport_protocol::TCP)
		{
			FD_SET(serverSocket[1], &fread); // in a listening socket only the fread and fexception are used
			FD_SET(serverSocket[1], &fexception);

			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 1;
			int nfds{ 0 }; //Ignored. The nfds parameter is included only for compatibility with Berkeley sockets. https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select

			auto s = select(nfds, &fread, &fwrite, &fexception, &tv); // pull to check if there is any connection request
			if (s > 0)
			{
				process = ipTunnelRecv_tcp(space);
			}
		}
		else 
		{
			process = ipTunnelRecv_udp(space);
		}
		
		if (process == 0) return false;

		if (getVerboseMode()) 
			std::cout << "Samples received through IP Tunnel: " << process << std::endl;
	
		if (outputSignals[0]->getValueType() == (signal_value_type::t_message))
		{
			size_t aux{ 0 };
			while (aux < process)
			{
				t_message msg;
				msg.messageRaw.resize(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE);

				for (auto k = 0; k < MESSAGE_TYPE_SIZE; k++)
					msg.messageRaw[k] = ip_tunnel_buffer[aux+k];
				for (auto k = 0; k < MESSAGE_DATA_LENGTH_SIZE; k++)
					msg.messageRaw[MESSAGE_TYPE_SIZE+k] = ip_tunnel_buffer[aux+MESSAGE_TYPE_SIZE + k];
				size_t dataLength = strToNumber(msg.getMessageDataLength());
				msg.messageRaw.resize(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE+dataLength);
				for (auto k = 0; k < dataLength; k++)
					msg.messageRaw[MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE + k] = ip_tunnel_buffer[aux+MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE + k];

				for (auto k : outputSignals)
				{
					k->bufferPut(msg);
				}

				aux = aux + msg.size();
			}
		}
		else
		{
			size_t aux = (process * sizeof(char) / outputSignals[0]->valueTypeSizeOf());
			for (auto k : outputSignals) 
			{
				k->bufferPut((std::byte*) &(ip_tunnel_buffer[0]), aux);
			}
		}
		}
	}
	// Run Client/Transmitter
	else {
		if (!connectionEstablished)
		{
			sockaddr_in hint;
			hint.sin_family = AF_INET;
//			hint.sin_port = htons((u_short)localMachinePort);
			hint.sin_port = htons((u_short)remoteMachinePort);
			inet_pton(AF_INET, remoteMachineIpAddress.c_str(), &hint.sin_addr);
//			inet_pton(AF_INET, localMachineIpAddress.c_str(), &hint.sin_addr);
			int connResult = connect(clientSocket[0], (sockaddr*)&hint, sizeof(hint));

			if (connResult == SOCKET_ERROR)
			{
				if (getVerboseMode())
				{
					std::cout << "Not connected to the server, yet!" << std::endl;
				}
				return alive;
			}
			else
			{
				if (getVerboseMode())
				{
					std::cout << "Already connected to the server, " << remoteMachineIpAddress << ", on port " << remoteMachinePort << "!" << std::endl;
				}
				connectionEstablished = true;
			}
		}
		else
		{
		
			size_t ready = inputSignals[0]->ready();

			if (ready == 0) return false;

			if (inputSignals[0]->getValueType() == (signal_value_type::t_message))
			{
				ip_tunnel_buffer.resize(0);
				t_message msg;
//				for (auto k = 0; k < ready; k++)
//				{
					inputSignals[0]->bufferGet((t_message *) &msg);
					std::copy(msg.messageRaw.begin(), msg.messageRaw.end(), std::back_inserter(ip_tunnel_buffer));
//				}
				process = ip_tunnel_buffer.size();
			}
			else
			{
				inputSignals[0]->bufferGet((std::byte*) &(ip_tunnel_buffer[0]), ready);
				process = ready * inputSignals[0]->valueTypeSizeOf() / sizeof(char);
			}

			if (transportProtocol == transport_protocol::TCP) {
				process=ipTunnelSend_tcp(process);
			}
			else {
				process = ipTunnelSend_udp(process);
			};

			if (process == 0) return false;

			if (getVerboseMode()) std::cerr << "Samples send through IP Tunnel: " << process << std::endl;
		}
	}

	return true;
}


void IPTunnel::terminate(void) {
	for (auto k = clientSocket.begin(); k!=clientSocket.end(); k++)	close(*k);
	for (auto k = serverSocket.begin(); k != serverSocket.end(); k++) close(*k);
}

size_t IPTunnel::ipTunnelSend_tcp(size_t process) {
	size_t remaining = process;
	size_t result{ 0 };
	size_t sent{ 0 };
//	while (remaining > 0) {
		result = send(clientSocket[0], &(ip_tunnel_buffer[0]) + sent, (int) remaining, 0);
		if (result >= 0) {
			remaining -= result;
			sent += result;
			if (getVerboseMode())
			{
				std::cout << result << " bytes sent through TCP" << std::endl;
			}
		}
		else { 
			if (getLogValue()) {
				if ( result == 0) std::cerr << "No data send through TCP" << std::endl;
                		else std::cerr << "ERROR sending TCP, error #: " << errno << std::endl;
			}
//			break;
//		}
	}
	return sent;
}



size_t IPTunnel::ipTunnelRecv_tcp(size_t space) {
	char* recv_buffer = &ip_tunnel_buffer[0];
	size_t remaining{ 0 };
	if (outputSignals[0]->getValueType() == (signal_value_type::t_message))
	{
		remaining = ip_tunnel_buffer.size();
	}
	else
	{
		remaining = space * outputSignals[0]->valueTypeSizeOf();
	}
	size_t received{ 0 };
//	while (remaining > 0) {
		auto aux{ 0 };
		aux = recv(serverSocket[1], recv_buffer + received, (int) remaining, 0);
		if (aux > 0) {
			received += aux;
			remaining -= received;
			if (getVerboseMode())
			{
				std::cout << aux << " bytes received through TCP" << std::endl;
			}
		}
		else {
			if (getVerboseMode()) {
				if (aux == 0) std::cerr << "No data received through TCP" << std::endl;
               			else std::cerr << "ERROR receiving TCP, error #: " << errno << std::endl;
			}
//			break;
//		}
	}
	return received;
}


bool IPTunnel::server_tcp() {

// # Steps to create the TCP Server  ##########################################
//
// 1 - Create a socket
// 2 - Bind the ip address and port to a socket
// 3 - Listen on the socket for a client
// 4 - Accept a connection from a client
// https://gist.github.com/codehoose/020c6213f481aee76ea9b096acaddfaf
//
// ############################################################################


// # STEP 1 - Create a socket #################################################				

	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
//		return false;
		exit(1);
	}
	if (getVerboseMode())
	{
		std::cout << "The Server Socket was created." << std::endl;
	}
	serverSocket.push_back(s);
	
// # STEP 2 - Bind the ip address and port to a socket ########################

	sockaddr_in hint;
	hint.sin_family = AF_INET; // AF_INET=2, IPv4
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.s_addr);
//    	inet_pton(AF_INET, remoteMachineIpAddress.data(), &hint.sin_addr.s_addr);
//	hint.sin_port = ntohs((u_short)remoteMachinePort);
	hint.sin_port = ntohs((u_short)localMachinePort);	
	memset(&(hint.sin_zero), 0, 8);
auto bind_val = bind(serverSocket[0], (sockaddr*)& hint, sizeof(hint));
std::cout << "bind is " << bind_val << std::endl;
	std::cout << "The  identified socket that should be unbound is " << serverSocket[0] << std::endl;
//	if (bind(serverSocket[0], (sockaddr*)& hint, sizeof(hint)) < 0) {
if (bind_val<0){
        	std::cerr << "Bind failed with error code #" << errno << std::endl;
//		return false;
		exit(1);
	}

	char ipAddress[INET_ADDRSTRLEN];
    	inet_ntop(AF_INET, &hint.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
	if (getVerboseMode())
	{
		std::cout << "The TCP server is running on IP address: " << ipAddress;
		std::cout << " Port: " << ntohs(hint.sin_port) << std::endl;
	}


// # STEP 3 - Listen on the socket for a client ###############################

	if (listen(serverSocket[0], SOMAXCONN) == -1) {
		std::cerr << "Listen error!" << std::endl;
		return false;
		exit(1);
	}
	if (getVerboseMode())
	{
		std::cout << "The Server is listenning on local port: " << ntohs(hint.sin_port) << std::endl;
	}

// # STEP 4 - Accept a connection from a client ###############################

//	sockaddr_in client;
//	socklen_t clientSize = sizeof(client);

//	s = accept(serverSocket[0], (sockaddr*) &client, &clientSize);
//	serverSocket.push_back(s);

//	// Provides information
//	char host[NI_MAXHOST];  // Client's remote name
//	char service[NI_MAXSERV]; // Service (i.e. port) the client is connect on

//	//ZeroMemory(host, NI_MAXHOST);
//	//ZeroMemory(service, NI_MAXSERV);
//	memset(host, 0, NI_MAXHOST);
// 	memset(service, 0, NI_MAXSERV);

//	if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
//	{
//		std::cerr << host << " connected on port " << service << std::endl;
//	}
//	else
//	{
//		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//		std::cerr << host << " connected on port " << ntohs(client.sin_port) << std::endl;
//	}
	
	return true;
}

bool IPTunnel::client_tcp() {

// # Steps to create the TCP Client  ##########################################
//
// 1 - Create a socket
// 2 - Create a hint structure for the server we're connecting to
// 3 - Connect to the server on the socket
// https://gist.github.com/codehoose/d7dea7010d041d52fb0f59cbe3826036
//
// ############################################################################


// # STEP 1 - Create a socket #################################################						// 
	
	int s = socket(AF_INET, SOCK_STREAM, 0);
	clientSocket.push_back(s);
		
	if (clientSocket[0] == INVALID_SOCKET)
	{
        std::cerr << "Can't create socket, Error Code #" << errno  << std::endl;
		return false;
	}
	
// # STEP 2 - Create a hint structure for the server we're connecting to ######

//	sockaddr_in hint;
//	hint.sin_family = AF_INET;
//	hint.sin_port = htons((u_short)localMachinePort);
//	inet_pton(AF_INET, remoteMachineIpAddress.c_str(), &hint.sin_addr);

// # STEP 3 - Connect to the server on the socket #############################

//	int connResult{ -2 };
//	while (connResult != 0 || numberOfTrials == 0) {
//		connResult = connect(clientSocket[0], (sockaddr*)& hint, sizeof(hint));
//		if (connResult == SOCKET_ERROR)
//		{
//            std::cerr << "Can't connect to server, Err #" << errno << std::endl;
//			std::cerr << "Waiting " << timeIntervalSeconds << " seconds." << std::endl;
//            usleep(timeIntervalSeconds * 1000000);
//		}

//		if (--numberOfTrials == 0) {
//			std::cerr << "Reached maximum number of attempts." << std::endl;
//		}
//	}
//	std::cerr << "Connected!\n";
	return true;
}



size_t IPTunnel::ipTunnelSend_udp(size_t process) {

	sockaddr_in hint;
	hint.sin_family = AF_INET; 
	inet_pton(AF_INET, remoteMachineIpAddress.data(), &hint.sin_addr.s_addr);
	hint.sin_port = ntohs((u_short)remoteMachinePort);

	size_t remaining = process;
	size_t result{ 0 };
	size_t sent{ 0 };
	while (remaining > 0) {
		result = sendto(clientSocket[0], &(ip_tunnel_buffer[0]) + sent, (int)remaining, 0, (sockaddr*) &hint, INET_ADDRSTRLEN);
		if (result > 0) {
			remaining -= result;
			sent += result;
		}
		else {
			if (getLogValue()) {
				if (result == 0) std::cerr << "No data send through UDP" << std::endl;
				else std::cerr << "ERROR sending UDP, error #: " << errno << std::endl;
			}
			break;
		}
	}
	return sent;
}


size_t IPTunnel::ipTunnelRecv_udp(size_t space) {

	sockaddr_in senderAddress;
	unsigned int senderAddressSize = sizeof(senderAddress);

	char* recv_buffer = &ip_tunnel_buffer[0];
	size_t remaining = space * outputSignals[0]->valueTypeSizeOf();
	size_t received{ 0 };
	while (remaining > 0) {
		auto aux{ 0 };
		aux = recvfrom(serverSocket[0], recv_buffer + received, (int) remaining, 0, (sockaddr *) &senderAddress, &senderAddressSize);
		if (aux > 0) {
			received += aux;
			remaining -= received;
		}
		else {
			if (getLogValue()) {
				if (aux == 0) std::cerr << "No data received through UDP" << std::endl;
				else std::cerr << "ERROR receiving UDP, error #: " << errno << std::endl;
			}
			break;
		}
	}
	return received;
}


bool IPTunnel::server_udp() {

	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	serverSocket.push_back(s);

	if (serverSocket[0] == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		
		return false;
	}


	sockaddr_in hint;
	hint.sin_family = AF_INET; // AF_INET=2, IPv4
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.s_addr);
	hint.sin_port = ntohs((u_short)localMachinePort);	// The htons() function converts the unsigned short integer hostshort from host byte order to network byte order. On the i386 the host byte order is Least Significant Byte first, whereas the network byte order, as used on the Internet, is Most Significant Byte first.

	char ipAddress[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &hint.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
	std::cerr << "The UDP server is running on IP address: " << ipAddress;
	std::cerr << " Port: " << htons(hint.sin_port) << std::endl;
	std::cout << "not print" << std::endl;
	if (bind(serverSocket[0], (sockaddr*)& hint, sizeof(hint)) == SOCKET_ERROR) {
		std::cerr << "Bind failed with error code #" << errno << std::endl;
		return false;
	}
	
	return true;
}


bool IPTunnel::client_udp() {

	
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	clientSocket.push_back(s);

	if (clientSocket[0] == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		return false;
	}

	std::cerr << "The UDP client socket was created!" << std::endl;

	return true;
}
