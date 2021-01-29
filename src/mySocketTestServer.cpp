#include "mySocket.h"

void errorHandler(std::string message);

int main(void) {
	short port;
	std::string buffer;

	std::cout << "insert port : ";
	std::cin >> port;

	MySocket servSock(TCP);
	if (servSock.bind(port) == -1)
		errorHandler("bind() error");

	if (servSock.listen(5) == -1)
		errorHandler("bind() error");

	MySocket clntSock = servSock.accept();

	while (1) {
		buffer = clntSock.recv();
		clntSock.send(buffer);
	}

	clntSock.close();
	return 0;
}

void errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
