#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <memory.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 30

void errorHandler(std::string message);

int main(void) {
	short port;

	std::cout << "insert port : ";
	std::cin >> port;

	int servSock;
	sockaddr_in servAddr, clntAddr;
	socklen_t clntAddrSize;

	char buffer[BUFFER_SIZE];

	servSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (servSock == -1) {
		errorHandler("socket() error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(servSock, (sockaddr *)&servAddr, sizeof(servAddr)) == -1)
		errorHandler("bind() error");
	
	int recvSize;

	while (1) {
		clntAddrSize = sizeof(clntAddr);
		recvSize = recvfrom(servSock, buffer, BUFFER_SIZE, 0, 
			(sockaddr *)&clntAddr, &clntAddrSize);
		sendto(servSock, buffer, recvSize, 0,
			(sockaddr *)&clntAddr, clntAddrSize);
	}

	close(servSock);
	return 0;
}


void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
