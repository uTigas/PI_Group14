# include "ip_tunnel_ms_windows_20200819.h" 

#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <chrono>

// ############################################################
//  these constants are useful, but appear to be specific to */
//  winsock; so, I define them here and use them as if they */
//  were standard */
#define INVALID_SOCKET ((~0))
#define SOCKET_ERROR (-1)
// ############################################################
#endif

// Message Handler Messages data
const uint32_t HEADER_SIZE = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE +
                             ID_SIZE + MSG_TYPE_SIZE + SRC_PORT_SIZE +
                             MSG_DATA_LENGTH_SIZE;
const uint32_t MAX_COMPLETE_MSG_SIZE = HEADER_SIZE + MAX_MSG_SIZE;


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
				#ifdef _WIN32
				exit(ERROR);
				#endif

				#ifdef __linux__
				exit(1);
				#endif
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
		else if (outputSignals[0]->getValueType() == (signal_value_type::t_handler_message))
			ip_tunnel_buffer.resize(outputSignals[0]->getBufferLength()*MAX_MSG_SIZE);
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

				#ifdef _WIN32
				exit(ERROR);
				#endif

				#ifdef __linux__
				exit(1);
				#endif
			}
		}
		else {
			std::cerr << "UDP Client" << std::endl;
			if (!client_udp()) std::cerr << "Error opening client\n";
		}
		// Resize the local buffer
		if (inputSignals[0]->getValueType()==(signal_value_type::t_message))
			ip_tunnel_buffer.resize(inputSignals[0]->getBufferLength()*MAX_MESSAGE_SIZE);
		else if (inputSignals[0]->getValueType() == (signal_value_type::t_handler_message))
			ip_tunnel_buffer.resize(inputSignals[0]->getBufferLength()*MAX_MSG_SIZE);
		else
			ip_tunnel_buffer.resize(inputSignals[0]->getBufferLength()*inputSignals[0]->valueTypeSizeOf() / sizeof(char));
	}
}


bool IPTunnel::runBlock(void)
{
	t_bool alive{ true };
	size_t process{ 0 };

	std::this_thread::sleep_for(std::chrono::milliseconds(timeIntervalSeconds));

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
			int nfds{ 0 }; //Ignored. The nfds parameter is included only for compatibility with Berkeley sockets. https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select

			auto s = select((int) serverSocket[0] + 1, &fread, &fwrite, &fexception, &tv); // pull to check if there is any connection request
			if (s > 0)
			{
				sockaddr_in client;
				int clientSize = sizeof(client);
				#ifdef _WIN32
				SOCKET s = accept(serverSocket[0], (sockaddr*)&client, (socklen_t *)(&clientSize));
				#endif

				#ifdef __linux__
				int s = accept(serverSocket[0], (sockaddr*)&client, (socklen_t *)(&clientSize));
				#endif
				if (s == INVALID_SOCKET)
				{
					#ifdef _WIN32
					std::cerr << "accept failed with error no. : " << WSAGetLastError << std::endl;
					WSACleanup();
					exit( ERROR );
					#endif

					#ifdef __linux__
					std::cerr << "accept failed with error no. : " << errno << std::endl;
					exit( 1 );
					#endif
				}
				else
				{
					serverSocket.push_back(s);
					connectionEstablished = true;
					if (getVerboseMode())
					{
						char ipAddress[INET_ADDRSTRLEN];
						#ifdef _WIN32
						inet_ntop(AF_INET, &client.sin_addr.S_un.S_addr, ipAddress, INET_ADDRSTRLEN);
						#endif
						
						#ifdef __linux__
						inet_ntop(AF_INET, &client.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
						#endif
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
			#ifdef _WIN32
			size_t space = outputSignals[0]->space();
			for (auto k : outputSignals)
				space = min(space, k->space());
			#endif

			#ifdef __linux__
			int space = outputSignals[0]->space();
			for (auto k : outputSignals)
				space = std::min(space, k->space());
			#endif

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

				auto s = select((int) serverSocket[1] + 1, &fread, &fwrite, &fexception, &tv); // pull to check if there is any connection request
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
						msg.messageRaw[k] = ip_tunnel_buffer[aux + k];
					for (auto k = 0; k < MESSAGE_DATA_LENGTH_SIZE; k++)
						msg.messageRaw[MESSAGE_TYPE_SIZE + k] = ip_tunnel_buffer[aux + MESSAGE_TYPE_SIZE + k];
					size_t dataLength = strToNumber(msg.getMessageDataLength());
					msg.messageRaw.resize(MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE + dataLength);
					for (auto k = 0; k < dataLength; k++)
						msg.messageRaw[MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE + k] = ip_tunnel_buffer[aux + MESSAGE_TYPE_SIZE + MESSAGE_DATA_LENGTH_SIZE + k];

					for (auto k : outputSignals)
					{
						k->bufferPut(msg);
					}

					aux = aux + msg.size();
				}
			}
			else if (outputSignals[0]->getValueType() == (signal_value_type::t_handler_message))
			{
				size_t aux{ 0 };

				while (aux < process)
				{
					t_handler_message msg;
					t_string rawMsg = msg.getRawMessage();
					rawMsg.resize(MAX_COMPLETE_MSG_SIZE);
					// Destination Block
					t_integer offset = 0;
					for (auto k = 0; k < DST_BLOCK_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Source Block
					offset = DST_BLOCK_SIZE;
					for (auto k = 0; k < SRC_BLOCK_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Priority
					offset = DST_BLOCK_SIZE + SRC_BLOCK_SIZE;
					for (auto k = 0; k < PRIORITY_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Sequential ID
					offset = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE;
					for (auto k = 0; k < ID_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Message Type Size
					offset = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE + ID_SIZE;
					for (auto k = 0; k < MSG_TYPE_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Source Port Size
					offset = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE + ID_SIZE + MSG_TYPE_SIZE;
					for (auto k = 0; k < SRC_PORT_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Data Size
					offset = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE + ID_SIZE + MSG_TYPE_SIZE + SRC_PORT_SIZE;
					for (auto k = 0; k < MSG_DATA_LENGTH_SIZE; k++)
						rawMsg[offset + k] = ip_tunnel_buffer[(aux + offset) + k];
					// Message Data
					msg.setRawMessage(rawMsg);
					size_t dataLength = std::stoi(msg.getMessageDataLength());
					rawMsg.resize(HEADER_SIZE + dataLength);
					for (auto k = 0; k < dataLength; k++)
						rawMsg[HEADER_SIZE + k] = ip_tunnel_buffer[(aux + HEADER_SIZE) + k];

					// Populate class
					msg.setRawMessage(rawMsg);

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
					k->bufferPut((std::byte*)&(ip_tunnel_buffer[0]), aux);
				}
			}
		}
	}
	// Run Client/Transmitter
	else 
	{
		if (!connectionEstablished)
		{
			sockaddr_in hint;
			hint.sin_family = AF_INET;
//			hint.sin_port = htons((u_short)localMachinePort);
			hint.sin_port = htons((u_short)remoteMachinePort);
			inet_pton(AF_INET, remoteMachineIpAddress.c_str(), &hint.sin_addr);
			int connResult = connect(clientSocket[0], (sockaddr*)&hint, sizeof(hint));

			if (connResult == SOCKET_ERROR)
			{
				if (getVerboseMode())
				{
					std::cout << "Not connected to the server, yet!" << std::endl;
					fprintf(stderr, "socket connect failed: %d \n", errno);
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
					inputSignals[0]->bufferGet((t_message*)&msg);
					std::copy(msg.messageRaw.begin(), msg.messageRaw.end(), std::back_inserter(ip_tunnel_buffer));
//				}
				process = ip_tunnel_buffer.size();
			}
			else if (inputSignals[0]->getValueType() == (signal_value_type::t_handler_message))
			{
				ip_tunnel_buffer.resize(0);
				t_handler_message msg;
//				for (auto k = 0; k < ready; k++)
//				{
					inputSignals[0]->bufferGet((t_handler_message*)&msg);
					t_string rawMsg = msg.getRawMessage();
					std::copy(rawMsg.begin(), rawMsg.end(), std::back_inserter(ip_tunnel_buffer));
//				}
				process = ip_tunnel_buffer.size();
			}
			else
			{
				inputSignals[0]->bufferGet((std::byte*)&(ip_tunnel_buffer[0]), ready);
				process = ready * inputSignals[0]->valueTypeSizeOf() / sizeof(char);
			}

			if (transportProtocol == transport_protocol::TCP) {
				process = ipTunnelSend_tcp(process);
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

	#ifdef _WIN32
	for (auto k = clientSocket.begin(); k!=clientSocket.end(); k++)	closesocket(*k);
	for (auto k = serverSocket.begin(); k != serverSocket.end(); k++) closesocket(*k);
	WSACleanup();					
	#endif

	#ifdef __linux__
	for (auto k = clientSocket.begin(); k!=clientSocket.end(); k++)	close(*k);
	for (auto k = serverSocket.begin(); k != serverSocket.end(); k++) close(*k);
	#endif
}

size_t IPTunnel::ipTunnelSend_tcp(size_t process) {
	size_t remaining = process;
	size_t result{ 0 };
	size_t sent{ 0 };
	// while (remaining > 0) {
		result = send(clientSocket[0], &(ip_tunnel_buffer[0]) + sent, (int) remaining, 0);
		if (result >= 0) 
		{
			remaining -= result;
			sent += result;

			if (getVerboseMode())
			{
				std::cout << result << " bytes sent through TCP" << std::endl;
			}

		}
		else { 
			if (getLogValue()) 
			{
				if ( result == 0) std::cerr << "No data send through TCP" << std::endl;
				#ifdef _WIN32
				else std::cerr << "ERROR sending TCP, error #: " << WSAGetLastError() << std::endl;
				#endif

				#ifdef __linux__
				else std::cerr << "ERROR sending TCP, error #: " << errno << std::endl;
				#endif
			}
			// break;
		}
	// }
	return sent;
}

size_t IPTunnel::ipTunnelSend_udp(size_t process) {

	sockaddr_in hint;
	hint.sin_family = AF_INET; 
	#ifdef _WIN32
	inet_pton(AF_INET, remoteMachineIpAddress.data(), &hint.sin_addr.S_un.S_addr);
	#endif

	#ifdef __linux__
	inet_pton(AF_INET, remoteMachineIpAddress.data(), &hint.sin_addr.s_addr);
	#endif
	hint.sin_port = ntohs((u_short)remoteMachinePort);

	size_t remaining = process;
	size_t result{ 0 };
	size_t sent{ 0 };
//	while (remaining > 0) {
		result = sendto(clientSocket[0], &(ip_tunnel_buffer[0]) + sent, (int)remaining, 0, (sockaddr*) &hint, INET_ADDRSTRLEN);
		if (result > 0) {
			remaining -= result;
			sent += result;
			if (getLogValue())
			{
				std::cout << result << " bytes sent through TCP" << std::endl;
			}
		}
		else {
			if (getVerboseMode()) {
				if (result == 0) std::cerr << "No data send through UDP" << std::endl;
				#ifdef _WIN32
				else std::cerr << "ERROR sending UDP, error #: " << WSAGetLastError() << std::endl;
				#endif

				#ifdef __linux__
				else std::cerr << "ERROR sending UDP, error #: " << errno << std::endl;
				#endif
			}
//			break;
		}
//	}
	return sent;
}

size_t IPTunnel::ipTunnelRecv_tcp(size_t space) {
	char* recv_buffer = &ip_tunnel_buffer[0];
	size_t remaining{ 0 };
	if (outputSignals[0]->getValueType() == (signal_value_type::t_message))
	{
		remaining = ip_tunnel_buffer.size();
	}
	else if (outputSignals[0]->getValueType() == (signal_value_type::t_handler_message))
	{
		remaining = ip_tunnel_buffer.size();
	}
	else
	{
		remaining = space * outputSignals[0]->valueTypeSizeOf();
	}
	size_t received{ 0 };
//	while (remaining > 0) 
	{
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
			if (getVerboseMode())
			{
				if (aux == 0) std::cerr << "No data received through TCP" << std::endl;
				#ifdef _WIN32
				else std::cerr << "ERROR receiving TCP, error #: " << WSAGetLastError() << std::endl;
				#endif

				#ifdef __linux__
				else std::cerr << "ERROR receiving TCP, error #: " << errno << std::endl;
				#endif
				
			}
//			break;
		}
	}
	return received;
}

size_t IPTunnel::ipTunnelRecv_udp(size_t space) {

	sockaddr_in senderAddress;
	int senderAddressSize = sizeof(senderAddress);

	char* recv_buffer = &ip_tunnel_buffer[0];
	size_t remaining = space * outputSignals[0]->valueTypeSizeOf();
	size_t received{ 0 };
	while (remaining > 0) {
		auto aux{ 0 };
		aux = recvfrom(serverSocket[0], recv_buffer + received, (int) remaining, 0, (sockaddr *) &senderAddress, (socklen_t*)(&senderAddressSize));
		if (aux > 0) {
			received += aux;
			remaining -= received;
		}
		else {
			if (getLogValue()) {
				if (aux == 0) std::cerr << "No data received through UDP" << std::endl;
				#ifdef _WIN32
				else std::cerr << "ERROR receiving UDP, error #: " << WSAGetLastError() << std::endl;
				#endif

				#ifdef __linux__
				else std::cerr << "ERROR receiving UDP, error #: " << errno << std::endl;
				#endif
				
			}
			break;
		}
	}
	return received;
}

bool IPTunnel::server_tcp() {

// Steps to create the TCP Server			// https://docs.microsoft.com/en-us/windows/win32/winsock
// 1 - Initialize Winsock				
// 2 - Create a socket
// 2b -	Initialize the socketaddr structure
// 3 - Bind the socket
// 4 - Listen on the socket for a client
// 5 - Accept a connection from a client (this part should be done in the run part)

	#ifdef _WIN32
	// STEP 1 - Initialize Winsock

	WSADATA wsData;								// The WSADATA structure stores information about the Windows Sockets implementation.
	WORD ver = MAKEWORD(2, 2);					// The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system
	int wsResult = WSAStartup(ver, &wsData);	// The WSAStartup function initiates the use of the Winsock library.
	if (wsResult != 0)							// If successful, the WSAStartup function returns zero. Otherwise, it returns a error code.
	{
		std::cerr << "Can't start Winsock. Error code #" << wsResult << std::endl;
		WSACleanup();
		exit(ERROR);
	}
	
	if (getVerboseMode())
	{
		std::cout << "Winsock started." << std::endl;
	}
	#endif

	// STEP 2 - Create a socket					

	// The socket function creates a socket that is bound to a specific transport service provider.
	// If no error occurs, socket returns a descriptor referencing the new socket.
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	//
	//	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	//	serverSocket: socket descriptor, an integer(like a file - handle)
	//	domain : integer, communication domain e.g., AF_INET(IPv4 protocol), AF_INET6(IPv6 protocol)
	//	type : communication type
	//		SOCK_STREAM : TCP(reliable, connection oriented)
	//		SOCK_DGRAM : UDP(unreliable, connectionless), SOCK_DGRAM = 2
	//	protocol : Protocol value for Internet Protocol(IP), which is 0. 
	//	This is the same number which appears on protocol field in the IP header of a packet.

	#ifdef _WIN32
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	#endif

	#ifdef __linux__
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	#endif
	if (s == INVALID_SOCKET)
	{
		#ifdef _WIN32
		std::cerr << "Can't create socket, Error Code #" << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(ERROR);
		#endif

		#ifdef __linux__
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		exit(1);
		#endif

	}

	if (getVerboseMode())
	{
		std::cout << "The Server Socket was created." << std::endl;
	}
	serverSocket.push_back(s);    // serverSocket contains a set of socket, the listening socket is the serverSocket[0] 

	// STEP 2b - Initialize the socketaddr structure

	sockaddr_in hint;
	hint.sin_family = AF_INET; // AF_INET=2, IPv4

	#ifdef _WIN32
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.S_un.S_addr);
	#endif // The InetPton function converts an IPv4 or IPv6 Internet network address in its standard text presentation form into its numeric binary form
	//hint.sin_addr.S_un.S_addr = INADDR_ANY; // We bind the server to all localhost addresses

	#ifdef __linux__
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.s_addr);
	#endif

	hint.sin_port = htons((u_short)localMachinePort);	// The htons() function converts the unsigned short integer hostshort from host byte order to network byte order. On the i386 the host byte order is Least Significant Byte first, whereas the network byte order, as used on the Internet, is Most Significant Byte first.
	memset(&(hint.sin_zero), 0, 8);

	// STEP 3 - Bind the socket
	// For a server to accept client connections, it must be bound to a network address and port.
	// After creation of the socket, bind function binds the socket to the address
	// and port number specified in hint(custom data structure).
	// We bind the server to the localhost, hence we use INADDR_ANY 
	// to specify the IP address

	if (bind(serverSocket[0], (sockaddr*)&hint, sizeof(hint)) < 0) 
	{
		#ifdef _WIN32
		std::cerr << "Bind failed with error code #" << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(ERROR);
		#endif

		#ifdef __linux__
		std::cerr << "Bind failed with error code #" << errno << std::endl;
		exit(1);
		#endif
	}

	char ipAddress[INET_ADDRSTRLEN];

	#ifdef _WIN32
	inet_ntop(AF_INET, &hint.sin_addr.S_un.S_addr, ipAddress, INET_ADDRSTRLEN);
	#endif

	#ifdef __linux__
	inet_ntop(AF_INET, &hint.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
	#endif

	if (getVerboseMode())
	{
		std::cout << "The TCP server is running on IP address: " << ipAddress;
		std::cout << " Port: " << ntohs(hint.sin_port) << std::endl;
	}

	//DWORD timeout = 5 * 1000;
	//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

	// STEP 4 - Start Listen on the socket for a client
	// It puts the server socket in a passive mode, where it waits for the client
	// to approach the server to make a connection.
	// The backlog, defines the maximum length to which the queue of pending connections may grow.
	// If a connection request arrives when the queue is full, the client may receive an error with
	// an indication of ECONNREFUSED.

	if (listen(serverSocket[0], SOMAXCONN) == -1) 
	{
		#ifdef _WIN32
		std::cerr << "Listen error!" << std::endl;
		WSACleanup();
		exit(ERROR);
		#endif

		#ifdef __linux__
		std::cerr << "Listen error!" << std::endl;
		exit(1);
		#endif
	}
	if (getVerboseMode())
	{
		std::cout << "The Server is listenning on local port: " << ntohs(hint.sin_port) << std::endl;
	}

	// STEP 5 - Accept a connection from a client
	// It extracts the first connection request on the queue of pending
	// connections for the listening socket, creates a new connected socket,
	// and returns a new socket descriptor referring to that socket.
	// At this point, connection is established between client and server,
	// and they are ready to transfer data

	//sockaddr_in client;
	//int clientSize = sizeof(client);
	//if (accept(serverSocket[0], (sockaddr*)&client, &clientSize) == INVALID_SOCKET)
	//	std::cerr << "accept failed with error no. : " << WSAGetLastError << std::endl;
	//else
	//	std::cout << "Client connected!!!" << std::endl;

	////s = accept(serverSocket[0], (sockaddr*)&client, &clientSize);
	////serverSocket.push_back(s);

	////if (s== INVALID_SOCKET) std::cerr << WSAGetLastError;

	//// Provides information
	//char host[NI_MAXHOST];
	//char service[NI_MAXSERV];

	//ZeroMemory(host, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	//if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	//{
	//	std::cout << host << " Connected on port " << service << std::endl;
	//}
	//else
	//{
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	std::cout << host << " Connected on port " << ntohs(client.sin_port) << std::endl;
	//}
	//closesocket(s);
	//WSACleanup();

	return true;
}

bool IPTunnel::server_udp() {

	// Steps to create the UDP Server			// https://docs.microsoft.com/en-us/windows/win32/winsock
	// 1 - Initialize Winsock				
	// 2 - Create a socket
	// 3 - Bind the socket

	#ifdef _WIN32
	// STEP 1 - Initialize Winsock
	WSADATA wsData;								// The WSADATA structure can store information about the Windows Sockets implementation.
	WORD ver = MAKEWORD(2, 2);					// The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system
	int wsResult = WSAStartup(ver, &wsData);	// The WSAStartup function initiates use of the Winsock DLL by a process.
	if (wsResult != 0)							// If successful, the WSAStartup function returns zero. Otherwise, it returns a error code.
	{
		std::cerr << "Can't start Winsock. Error code #" << wsResult << std::endl;
		return false;
	}
	#endif

	// STEP 2 - Create a socket					

	// The socket function creates a socket that is bound to a specific transport service provider.
	// If no error occurs, socket returns a descriptor referencing the new socket.
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	//
	//	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	//	serverSocket: socket descriptor, an integer(like a file - handle)
	//	domain : integer, communication domain e.g., AF_INET(IPv4 protocol), AF_INET6(IPv6 protocol)
	//	type : communication type
	//		SOCK_STREAM : TCP(reliable, connection oriented)
	//		SOCK_DGRAM : UDP(unreliable, connectionless), SOCK_DGRAM = 2
	//	protocol : Protocol value for Internet Protocol(IP), which is 0. 
	//	This is the same number which appears on protocol field in the IP header of a packet.

	#ifdef _WIN32
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#endif

	#ifdef __linux__
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#endif

	if (s == INVALID_SOCKET)
	{
		#ifdef _WIN32
		std::cerr << "Can't create socket, Error Code #" << WSAGetLastError() << std::endl;
		WSACleanup();
		#endif

		#ifdef __linux__
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		#endif
		exit(1);
	}
	serverSocket.push_back(s);

	// STEP 3 - Bind the socket
	// For a server to accept client connections, it must be bound to a network address and port.
	// After creation of the socket, bind function binds the socket to the address
	// and port number specified in hint(custom data structure).

	sockaddr_in hint;
	hint.sin_family = AF_INET; // AF_INET=2, IPv4

	#ifdef _WIN32
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.S_un.S_addr);
	#endif

	#ifdef __linux__
	inet_pton(AF_INET, localMachineIpAddress.data(), &hint.sin_addr.s_addr);
	#endif

	hint.sin_port = ntohs((u_short)localMachinePort);	// The htons() function converts the unsigned short integer hostshort from host byte order to network byte order. On the i386 the host byte order is Least Significant Byte first, whereas the network byte order, as used on the Internet, is Most Significant Byte first.

	char ipAddress[INET_ADDRSTRLEN];

	#ifdef _WIN32
	inet_ntop(AF_INET, &hint.sin_addr.S_un.S_addr, ipAddress, INET_ADDRSTRLEN);
	#endif

	#ifdef __linux__
	inet_ntop(AF_INET, &hint.sin_addr.s_addr, ipAddress, INET_ADDRSTRLEN);
	#endif

	std::cerr << "The UDP server is running on IP address: " << ipAddress;
	std::cerr << " Port: " << htons(hint.sin_port) << std::endl;

	if (bind(serverSocket[0], (sockaddr*)& hint, sizeof(hint)) == SOCKET_ERROR) {
		#ifdef _WIN32
		std::cerr << "Bind failed with error code #" << WSAGetLastError() << std::endl;
		#endif

		#ifdef __linux__
		std::cerr << "Bind failed with error code #" << errno << std::endl;
		#endif

		return false;
	}
	
	return true;
}

bool IPTunnel::client_tcp() {

	// Steps to create the Client				// https://docs.microsoft.com/en-us/windows/win32/winsock
	// 1 - Initialize Winsock
	// 2 - Create a socket
	// 3 - Connect to the server				// (this part should be done in the run part)


	#ifdef _WIN32
	// STEP 1 - Initialize Winsock
	WSADATA wsData;								// The WSADATA structure can store information about the Windows Sockets implementation.
	WORD ver = MAKEWORD(2, 2);					// The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system
	int wsResult = WSAStartup(ver, &wsData);	// The WSAStartup function initiates use of the Winsock DLL by a process.
	if (wsResult != 0)							// If successful, the WSAStartup function returns zero. Otherwise, it returns a error code.
	{
		std::cerr << "Can't start Winsock. Error code #" << wsResult << std::endl;
		return false;
	}
	#endif

	// STEP 2 - Create a socket					// After initialization, a SOCKET object must be instantiated for use by the client

	// The socket function creates a socket that is bound to a specific transport service provider.
	// If no error occurs, socket returns a descriptor referencing the new socket.
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	//
	//	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	//	clientSockey: socket descriptor, an integer(like a file - handle)
	//	domain : integer, communication domain e.g., AF_INET(IPv4 protocol), AF_INET6(IPv6 protocol)
	//	type : communication type
	//		SOCK_STREAM : TCP(reliable, connection oriented)
	//		SOCK_DGRAM : UDP(unreliable, connectionless)
	//	protocol : Protocol value for Internet Protocol(IP), which is 0. 
	//	This is the same number which appears on protocol field in the IP header of a packet.
	
	#ifdef _WIN32
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	#endif

	#ifdef __linux__
	int s = socket(AF_INET, SOCK_STREAM, 0);
	#endif

	clientSocket.push_back(s);
		
	if (clientSocket[0] == INVALID_SOCKET)
	{
		#ifdef _WIN32
		std::cerr << "Can't create socket, Error Code #" << WSAGetLastError() << std::endl;
		WSACleanup();
		#endif

		#ifdef __linux__
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		#endif

		return false;
	}

	// STEP 3 - Connect to the server

	//sockaddr_in hint;
	//hint.sin_family = AF_INET;
	//hint.sin_port = htons((u_short)localMachinePort);
	//inet_pton(AF_INET, remoteMachineIpAddress.c_str(), &hint.sin_addr);

	//int connResult{ -2 };
	//while (connResult != 0 || numberOfTrials == 0) {
		//connResult = connect(clientSocket[0], (sockaddr*)& hint, sizeof(hint));
		//if (connResult == SOCKET_ERROR)
		//{
			//std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
			//std::cerr << "Waiting " << timeIntervalSeconds << " seconds." << std::endl;
		//}

		//if (--numberOfTrials == 0) {
			//std::cerr << "Reached maximum number of attempts." << std::endl;
		//}
	//}
	//if (getVerboseMode())
	//{
		//std::cout << "Connected!" << std::endl;
	//}
	////closesocket(s);
	////WSACleanup();
	return true;
}

bool IPTunnel::client_udp() {

	// Steps to create the Client				// https://docs.microsoft.com/en-us/windows/win32/winsock
	// 1 - Initialize Winsock
	// 2 - Create a socket
	// 3 - Bind the socket
	
	#ifdef _WIN32
	// STEP 1 - Initialize Winsock
	WSADATA wsData;								// The WSADATA structure can store information about the Windows Sockets implementation.
	WORD ver = MAKEWORD(2, 2);					// The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system
	int wsResult = WSAStartup(ver, &wsData);	// The WSAStartup function initiates use of the Winsock DLL by a process.
	if (wsResult != 0)							// If successful, the WSAStartup function returns zero. Otherwise, it returns a error code.
	{
		std::cerr << "Can't start Winsock. Error code #" << wsResult << std::endl;
		return false;
	}
	#endif

	// STEP 2 - Create a socket					// After initialization, a SOCKET object must be instantiated for use by the client

	// The socket function creates a socket that is bound to a specific transport service provider.
	// If no error occurs, socket returns a descriptor referencing the new socket.
	// Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	//
	//	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	//	clientSockey: socket descriptor, an integer(like a file - handle)
	//	domain : integer, communication domain e.g., AF_INET(IPv4 protocol), AF_INET6(IPv6 protocol)
	//	type : communication type
	//		SOCK_STREAM : TCP(reliable, connection oriented)
	//		SOCK_DGRAM : UDP(unreliable, connectionless)
	//	protocol : Protocol value for Internet Protocol(IP), which is 0. 
	//	This is the same number which appears on protocol field in the IP header of a packet.

	#ifdef _WIN32
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#endif

	#ifdef __linux__
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#endif

	clientSocket.push_back(s);

	if (clientSocket[0] == INVALID_SOCKET)
	{
		#ifdef _WIN32
		std::cerr << "Can't create socket, Error Code #" << WSAGetLastError() << std::endl;
		WSACleanup();
		#endif
		#ifdef __linux__
		std::cerr << "Can't create socket, Error Code #" << errno << std::endl;
		#endif

		return false;
	}

	std::cerr << "The UDP client socket was created!" << std::endl;

	return true;
}

