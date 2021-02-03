#include "../headers/mySocket.h"
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <atomic>
#include <chrono>

struct client
{
	bool isConnected = false;
	std::shared_ptr<MySocket> sock;
	
	client(bool a, std::shared_ptr<MySocket> b) {
		isConnected = a;
		sock = b;
	}
};

void getConnectLoop();
void getConnect();
void clientSupport(int index);

MySocket sock(TCP);
std::vector<client> clients;
std::vector<std::future<void>> threads;
int clientSize = 16;

std::recursive_mutex socketVectorLock;
std::recursive_mutex socketThreadLock;


int main(void) {
	short port;
	clients.reserve(clientSize);

	for (int i = 0; i < clientSize; i++) {
		clients[i].isConnected = false;
		clients[i].sock = NULL;
	}

	while (1) {
		std::cout << "insert port number(1024 ~ 49151) : ";
		std::cin >> port;
		if (port >= 1024 && port <= 49151)
			break;
		else
			std::cout << "port number invalid" << std::endl;
	}
	
	sock.bind(port);
	sock.listen(clientSize);

	std::cout << "Server Start" << std::endl;

	getConnect();

	auto acceptThread = std::async(getConnectLoop);


	for (int i = 0; i <= clientSize; i++){
		if (clientSize == i) {
			i = 0;
		}

		auto status = threads[i].wait_for(std::chrono::microseconds(0));
		if (status == std::future_status::ready) {
			std::cout << i << " : Thread finished" << std::endl;
			threads[i].get();
		}
	}

	acceptThread.get();
	
}

void getConnectLoop() {
	std::cout << "Loop Start" << std::endl;
	sleep(1);
	while (1) {
		getConnect();
	}
	std::cout << "Server End" << std::endl;
}

void getConnect() {
	int i = 0;
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		for (i = 0; i < clientSize; i++) {
			if (!clients[i].isConnected) {
				clients[i].isConnected = true;
				clients[i].sock = std::make_shared<MySocket>(sock.recv());
			}
		}
	}

	{
		std::lock_guard<std::recursive_mutex> thread_lock(socketThreadLock);
		threads[i] = std::async(clientSupport, i);
	}
	std::cout << "Hello! " << i << std::endl;
	return;
}

void clientSupport(int index) {
	std::string buffer;
	std::cout << "in" << std::endl;

	bool isConnected;
	MySocket clnt(TCP);
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		isConnected = clients[index].isConnected;
		clnt = *clients[index].sock;
	}

	while (1) {
		buffer = clnt.recv();
		
		// when client shutdown
		if (buffer.size() == 1 || buffer.substr(0, 7) == "::end::") {
			for (auto a : clients) {
				std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
				a.sock -> send(buffer.substr(7) + " is out");
			}
			break;
		}

		//send message to every clients
		{
			std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
			for (auto a : clients) {
				a.sock->send(buffer);
			}
		}
	}

	clnt.close();

	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		clients[index].isConnected = false;
		clients[index].sock = NULL;
	}

	std::cout << index << " good bye!" << std::endl;
}
