#include "mySocket.h"

int main(void) {

	unsigned short port = 0;
	std::string buffer;

	while (1) {
		std::cout << "insert port number(1024 ~ 49151) : ";
		std::cin >> port;
		if (port >= 1024 && port <= 49151)
			break;
		else
			std::cout << "port number invalid" << std::endl;
	}

	MySocket sock(UDP);

	sock.debugMode = true;

	sock.bind(port);
	sock.listen(5);

	for (int i = 0; i < 5; i++) {
		while (1) {
			SockAddrSet clnt;
			buffer = sock.recvFrom(clnt);

			if (buffer.size() == 1)
				break;

			sock.sendTo(buffer, clnt);
		}
	}

	sock.close();
	return 0;
}