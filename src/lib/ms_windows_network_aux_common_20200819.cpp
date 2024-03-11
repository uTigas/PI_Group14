#include "ms_windows_network_aux_common_20200819.h"

std::string NetworkAuxCommon::getIp(void) {
	#ifdef _WIN32
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	ULONG family = AF_INET;
	char buff[100];
	DWORD bufflen = 100;
	std::string IPV4;


	// To retrieve hostname 
	ULONG  outBufLen = 15000;
	pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
	if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
		free(pAddresses);
		pAddresses = NULL;
	}
	else {
		pCurrAddresses = pAddresses;
		if (pCurrAddresses != NULL) {
			pUnicast = pCurrAddresses->FirstUnicastAddress;

			if (pUnicast != NULL) {
				sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
				IPV4 = inet_ntop(AF_INET, &(sa_in->sin_addr), buff, bufflen);
				//				for (i = 0; pUnicast != NULL; i++)
				//					pUnicast = pUnicast->Next;
			}
		}
		free(pAddresses);
	}

	return IPV4;
	#endif




	#ifdef __linux__ 
	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;


	getifaddrs(&ifAddrStruct);

        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
           continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
           printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } 
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return std::string(ifa->ifa_name);
	#endif
}
