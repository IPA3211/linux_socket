#include "mySocket.h"

int main(void) {
	std::string ip;
	short port;
	std::string buffer;

	std::cout << "Insert server IP : ";
	std::cin >> ip;
	std::cout << "insert port : ";
	std::cin >> port;

	MySocket servSock(UDP);

	while (1) {
		std::cout << "send message : ";
		std::cin >> buffer;

		if (buffer.size() == 1) {
			if (buffer[0] == 'q' || buffer[0] == 'Q') {
				break;
			}
		}

		servSock.sendTo(buffer, SockAddrSet(ip, port));

		std::cout << "echo : " << servSock.recv();
		std::cout << std::endl;
	}

	return 0;
}
