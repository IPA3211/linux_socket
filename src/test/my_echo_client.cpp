#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

void errorHandler(std::string message);

int main(void) {
	char readBuffer[BUFFER_SIZE];
	int recvSize;

	std::string buffer;
	std::string ip;
	short port;

	int servSock;
	sockaddr_in servAddr;

	std::cout << "Insert server IP : ";
	std::cin >> ip;
	
	std::cout << "Insert port number : ";
	std::cin >> port;

	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == -1) 
		errorHandler("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	if (connect(servSock, (sockaddr *)&servAddr, sizeof(servAddr)) == -1)
		errorHandler("connect() error");
	else
		std::cout << "connected...." << std::endl;

	while (1) {
		std::cout << "send message : ";
		std::cin >> buffer;

		if (buffer.size() == 1) {
			if (buffer[0] == 'q' || buffer[0] == 'Q')
				break;
		}

		write(servSock, buffer.c_str(), buffer.size());

		std::cout << "echo : ";

		while (1) {
			recvSize = read(servSock, readBuffer, BUFFER_SIZE - 1);

			if (recvSize == -1) {
				errorHandler("read() error");
			}
			
			readBuffer[recvSize] = 0;

			std::cout << readBuffer;

			if (recvSize < BUFFER_SIZE - 1) {
				break;
			}
		}

		std::cout << std::endl;
	}

	std::cout << "end";

	close(servSock);

	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
