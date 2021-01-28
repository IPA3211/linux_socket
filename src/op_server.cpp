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
#define OPSZ 4

void errorHandler(std::string message);
int calculate(int opnum, int opnds[], char op);

int main(void) {
	char buffer[BUFFER_SIZE];

	int servSock, clntSock;
	sockaddr_in servAddr, clntAddr;
	socklen_t clntAddrSize;
	int recvSize, result;
	char opndCnt;

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
		opndCnt = 0;
		clntSock = accept(servSock, (sockaddr *)&clntAddr, &clntAddrSize);

		if (clntSock == -1)
			errorHandler("accept() error");

		read(clntSock, &opndCnt, 1);

		int recv_len = 0;

		while ((opndCnt * OPSZ + 1) > recv_len) {
			recv_len += read(clntSock, &buffer[recv_len], BUFFER_SIZE - 1);
		}

		result = calculate(opndCnt, (int *)buffer, buffer[recv_len - 1]);
		write(clntSock, (char *)&result, sizeof(result));
		close(clntSock);
	}

	close(servSock);
	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}

int calculate(int opnum, int opnds[], char op) {
	int result = opnds[0], i;

	switch (op) {
	case '+':
		for (i = 1; i < opnum; i++) result += opnds[i];
		break;
	case'-':
		for (i = 1; i < opnum; i++) result -= opnds[i];
		break;
	case '*':
		for (i = 1; i < opnum; i++) result *= opnds[i];
		break;
	}
	return result;

}
