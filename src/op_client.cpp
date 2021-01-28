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
#define RLT_SIZE 4
#define OPSZ 4

void errorHandler(std::string message);

int main(void) {
	std::string buffer;
	std::string ip;
	short port;

	std::cout << "Insert server IP : ";
	std::cin >> ip;

	std::cout << "Insert port number : ";
	std::cin >> port;

	int servSock;
	sockaddr_in servAddr;

	char opmsg[BUFFER_SIZE];
	int opndCnt, result;

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

	std::cout << "Operand count : ";
	std::cin >> opndCnt;
	opmsg[0] = (char)opndCnt;

	for (int i = 0; i < opndCnt; i++) {
		std::cout << "Operand " << i << " : ";
		scanf("%d", (int *)&opmsg[i*OPSZ + 1]);
	}

	std::cout << "Operand count : ";
	std::cin >> opmsg[opndCnt*OPSZ + 1];

	write(servSock, opmsg, opndCnt*OPSZ + 2);
	read(servSock, &result, RLT_SIZE);

	std::cout << "result : " << result << std::endl;

	close(servSock);

	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
