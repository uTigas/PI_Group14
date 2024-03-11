#ifndef NETWORK_AUX_COMMON_H
#define NEWORK_AUX_COMMON_H
// Provides some basic networking functions that may be required outside of specialized
// blocks like IPTunnel
// For instance, finding the local IPAddress to show in the interface
// Windows only

#ifdef _WIN32
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <winsock2.h>
#endif

#ifdef __linux__
#include <cstddef> // to be checkd if all are required
#include <stdio.h>      
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <unistd.h> // to be checkd if all are required
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>

typedef unsigned long DWORD;
typedef void * HANDLE;
#endif

#include <string>

namespace NetworkAuxCommon {
	std::string getIp(void);
}


#endif // !NETWORK_AUX_COMMON_H



