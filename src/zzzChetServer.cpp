#include "mySocket.h"

int main(void) {

	unsigned short port = 0;

	while (1) {
		std::cout << "insert port number(1024 ~ 49151) : ";
			std::cin >> port;
		if (port >= 1024 && port <= 49151) 
			break;
		else
			std::cout << "port number invalid" << std::endl;
	}

	MySocket sock(TCP);

	sock.debugMode = true;

	sock.bind(port);
	sock.listen(5);

	for (int i = 0; i < 5; i++) {
		MySocket clnt = sock.accept();
		while (1) {
			clnt.send(clnt.recv());
		}
		clnt.close();
	}
	
	sock.close();
	return 0;
}