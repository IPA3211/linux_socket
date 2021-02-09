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
	std::string name;

	client(bool a, std::shared_ptr<MySocket> b) {
		isConnected = a;
		sock = b;
	}

	client() {
		isConnected = false;
		sock = NULL;
		name = "";
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
	clients.resize(clientSize);
	threads.resize(clientSize);

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

	auto acceptThread = std::async(getConnectLoop);

	for (int i = 0; i <= clientSize; i++) {
		if (clientSize == i) {
			i = 0;
		}
		if (threads[i].valid() == false) {
			continue;
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

#ifdef _WIN32
	Sleep(1000);
#else
	sleep(1);
#endif

	std::cout << "Server Start" << std::endl;
	while (1) {
		getConnect();
	}
	std::cout << "Server End" << std::endl;
}

void getConnect() {
	int i = 1000;
	std::shared_ptr<MySocket> temp(new MySocket(TCP));
	MySocket clnt = sock.accept();
	*temp = clnt;
	{
		std::cout << "1" << std::endl;
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		for (i = 0; i < clientSize; i++) {
			if (!clients[i].isConnected) {
				clients[i].isConnected = true;
				clients[i].sock = temp;
				break;
			}
		}
		std::cout << i << std::endl;
	}

	{
		std::cout << "3" << std::endl;
		std::lock_guard<std::recursive_mutex> thread_lock(socketThreadLock);
		threads[i] = std::async(clientSupport, i);
		std::cout << "4" << std::endl;
	}
	std::cout << "Hello! " << i << std::endl;
	return;
}

void clientSupport(int index) {
	std::string buffer;
	std::string name;
	std::cout << "in" << std::endl;

	bool isConnected;
	MySocket clnt(TCP);
	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		isConnected = clients[index].isConnected;
		clnt = *clients[index].sock;
	}

	name = clnt.recv();
	name.pop_back();

	if (name.size() <= 1) {
		clients[index].isConnected = false;
		clients[index].sock = NULL;
		clnt.close();
		return;
	}

	{
		std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
		clients[index].name = name;
	}

	clnt.send("well come " + name + "!");

	while (1) {
		buffer = clnt.recv();

		// when client shutdown
		if (buffer.size() <= 1 || buffer.substr(0, 7) == "::end::") {
			std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
			clients[index].isConnected = false;
			clients[index].sock = NULL;
			clients[index].name.clear();

			for (int i = 0; i < clientSize; i++) {
				if (clients[i].isConnected)
					clients[i].sock->send(name + " is out");
			}
			break;
		}

		//send message to every clients
		{
			std::lock_guard<std::recursive_mutex> vector_lock(socketVectorLock);
			for (int i = 0; i < clientSize; i++) {
				if (clients[i].isConnected)
					if (clients[i].sock->send("" + name + " : " + buffer) == -1) {
						clients[index].isConnected = false;
						clients[index].sock = NULL;
						clients[index].name.clear();

						for (int i = 0; i < clientSize; i++) {
							if (clients[i].isConnected)
								clients[i].sock->send(name + " is out");
						}
						break;
					}
			}
		}
	}

	clnt.close();

	std::cout << index << " good bye!" << std::endl;
}
