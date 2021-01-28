#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 30

void errorHandler(std::string message);

int main(void) {
	char readBuffer[BUFFER_SIZE];
	int recvSize;

	std::string buffer;
	std::string ip;
	short port;

	std::cout << "Insert server IP : ";
	std::cin >> ip;

	std::cout << "Insert port number : ";
	std::cin >> port;

	int servSock;
	sockaddr_in servAddr, fromAddr;
	socklen_t addrSize;

	servSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (servSock == -1)
		errorHandler("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	while (1) {
		std::cout << "send message : ";
		std::cin >> buffer;

		if (buffer.size() == 1) {
			if (buffer[0] == 'q' || buffer[0] == 'Q')
				break;
		}

		sendto(servSock, buffer.c_str(), buffer.size(), 0,
			(sockaddr *)&servAddr, sizeof(servAddr));

		addrSize = sizeof(fromAddr);
		recvSize = recvfrom(servSock, readBuffer, BUFFER_SIZE, 0,
			(sockaddr *)&fromAddr, &addrSize);

		readBuffer[recvSize] = 0;

		std::cout << "echo : " << readBuffer <<std::endl;
	}

	std::cout << "end";

	close(servSock);

	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
