#include "../headers/mySocket.h"
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

MySocket sock(TCP);
std::vector<std::shared_ptr<std::thread>> threads;
std::vector<std::shared_ptr<MySocket>> sockets;

std::recursive_mutex socketVectorLock;
std::recursive_mutex socketThreadLock;

void getConnectLoop();
void getConnect();
void client(MySocket clnt, int index);

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
	int index = 100;
	MySocket clnt = sock.accept();
	MySocket asd = clnt;
	std::shared_ptr<MySocket> sock(new MySocket(TCP));
	*sock = clnt;
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		index = sockets.size();
		sockets.push_back(sock);
	}
	
	std::shared_ptr<std::thread> thread(new std::thread(client, clnt, index));

	{
		std::lock_guard<std::recursive_mutex> thread_lock(socketThreadLock);
		threads.push_back(thread);
	}
	std::cout << "Hello! " << index << std::endl;
	return;
}

void client(MySocket clnt, int index) {
	std::string buffer;
	std::cout << "in" << std::endl;
	while (1) {
		buffer = clnt.recv();

		if (buffer == "")
			break;

		if (buffer.substr(0, 7) == "::end::") {
			{
				for (auto sock : sockets) {
				
					std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
					sock->send(buffer.substr(7) + " is out") == -1;

				}
			}
			break;
		}

		{
			std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
			for (auto sock : sockets) {
				sock->send(buffer);
			}
		}

	}

	clnt.close();
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		sockets.erase(sockets.begin() + index);
	}

	std::cout << index << " good bye!" << std::endl;
}
