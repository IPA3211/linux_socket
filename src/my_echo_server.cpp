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
	char buffer[BUFFER_SIZE];

	int servSock, clntSock;
	sockaddr_in servAddr, clntAddr;
	socklen_t clntAddrSize;
	int recvSize;

	short port;

	std::cout << "insert port : ";
	std::cin >> port;

	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == -1) {
		errorHandler("socket() error");
	}
	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(servSock, (sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
		errorHandler("bind() error");
	}

	if (listen(servSock, 5) == -1) {
		errorHandler("listen() error");
	}

	clntAddrSize = sizeof(clntAddr);

	for (int i = 0; i < 5; i++) {
		clntSock = accept(servSock, (sockaddr *)&clntAddr, &clntAddrSize);

		if (clntSock == -1)
			errorHandler("accept() error");

		std::cout << i << " client connected" << std::endl;

		while (1) {
			recvSize = read(clntSock, buffer, BUFFER_SIZE);
			
			if (recvSize == -1) {
				errorHandler("read() error");
			}

			if (recvSize == 0) {
				break;
			}

			write(clntSock, buffer, recvSize);
		}
		
		close(clntSock);

		std::cout << i << " bye" << std::endl;
	}

	close(servSock);
	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
