#include "../headers/mySocket.h"
#include <thread>
#include <atomic>
#include <termios.h>
#include <mutex>

void readServer(MySocket sock);
void writeServer(MySocket sock);
void screenRefresher();
int getch(void);

MySocket servSock(TCP);
std::string text;
std::string buffer;
std::string userName;

std::atomic<bool> isClosed(false);

std::recursive_mutex textLock;
std::recursive_mutex bufferLock;

int main(void) {
	std::string ip;
	short port;
	
	isClosed = 0;

	std::cout << "input server IP : ";
	std::cin >> ip;

	while (1) {
		std::cout << "insert port number(1024 ~ 49151) : ";
		std::cin >> port;
		if (port >= 1024 && port <= 49151)
			break;
		else
			std::cout << "port number invalid" << std::endl;
	}
	servSock.connect(ip, port);

	std::cout << "input your name : ";
	std::cin >> userName;

	servSock.send(userName);
	text = servSock.recv();
	
	if (text.size() == 1) {
		std::cout << "server Closed" << std::endl;
		servSock.close();
		return 0;
	}

	screenRefresher();

	std::thread read(readServer, servSock);
	std::thread write(writeServer, servSock);

	read.join();
	write.join();

	servSock.close();

	return 0;
}

void readServer(MySocket sock) {
	while (1) {
		if (isClosed.load())
			return;

		std::string servIn = sock.recv();

		if (servIn.size() == 1) {
			std::cout << "server Closed" << std::endl;
			isClosed.store(true, std::memory_order_release);
			continue;
		}

		{
			std::lock_guard<std::recursive_mutex> vector_lock(textLock);
			text = text + servIn + "\n";
		}
		screenRefresher();
	}
}

void writeServer(MySocket sock) {
	while (1) {
		if (isClosed.load())
			return;

		int input;
		input = getch();

		{
			std::lock_guard<std::recursive_mutex> vector_lock(bufferLock);
			if (input == '\n') {
				if (buffer.size() == 1) {
					if (buffer[0] == 'q' || buffer[0] == 'Q') {
						sock.send("::end::" + userName);
						isClosed.store(true, std::memory_order_release);
						continue;
					}
				}
				else if (buffer.size() != 0) {
					if (sock.send(buffer) == - 1) {
						isClosed.store(true, std::memory_order_release);
						continue;
					}
					buffer.clear();
				}
			}
			else {
				buffer.push_back(input);
				screenRefresher();
			}
		}
	}

	return;
}
void screenRefresher() {
	system("clear");
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	{
		std::lock_guard<std::recursive_mutex> vector_lock(textLock);
		std::cout << text;
	}
	std::cout << std::endl;
	std::cout << "send message : ";
	{
		std::lock_guard<std::recursive_mutex> vector_lock(bufferLock);
		std::cout << buffer << std::endl;
	}
}

int getch(void){
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}
