#include "../headers/mySocket.h"
#include <thread>
#include <mutex>
#include <vector>

MySocket sock(TCP);
std::vector<std::shared_ptr<std::thread>> threads;
std::vector<std::shared_ptr<MySocket>> sockets;

std::recursive_mutex socketVectorLock;
std::recursive_mutex socketThreadLock;

void getConnectLoop();
void getConnect();
void client(std::shared_ptr<MySocket> clnt, int index);

int main(void) {
	short port;

	while (1) {
		std::cout << "insert port number(1024 ~ 49151) : ";
		std::cin >> port;
		if (port >= 1024 && port <= 49151)
			break;
		else
			std::cout << "port number invalid" << std::endl;
	}
	
	sock.bind(port);
	sock.listen(5);

	getConnect();

	std::thread acceptLoop(getConnectLoop);

	for (auto thread : threads) {
		thread -> join();
	}

	acceptLoop.~thread();
}

void getConnectLoop() {
	while (1) {
		getConnect();
	}
}

void getConnect() {
	int index = 0;
	MySocket clnt = sock.accept();
	std::shared_ptr<MySocket> sock(new MySocket(clnt));

	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		index = sockets.size();
		sockets.push_back(sock);
	}

	std::shared_ptr<std::thread> thread(new std::thread(client, sock, index));
	{
		std::lock_guard<std::recursive_mutex> thread_lock(socketThreadLock);
		threads.push_back(thread);
	}
}

void client(std::shared_ptr<MySocket> clnt, int index) {
	std::string buffer;
	while (1) {
		buffer = clnt -> recv();

		if (buffer.compare("::end::") == 0) {
			break;
		}

		{
			std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
			for (auto sock : sockets) {
				sock->send(buffer);
			}
		}

	}

	clnt -> close();
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		sockets.erase(sockets.begin() + index);
	}
}