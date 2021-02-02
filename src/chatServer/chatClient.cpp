#include "../headers/mySocket.h"
#include <thread>
#include <termios.h>
#include <mutex>

void readServer(MySocket sock);
void writeServer(MySocket sock);
void screenRefresher();
int getch(void);
void changeInputType(bool type);

MySocket servSock(TCP);
std::string text;
std::string buffer;

std::recursive_mutex readLock;
std::recursive_mutex writeLock;

int main(void) {
	std::string ip;
	short port;
	

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

	changeInputType(true);

	std::thread read(readServer, servSock);
	std::thread write(writeServer, servSock);

	read.join();
	write.~thread();

	servSock.close();
}

void readServer(MySocket sock) {
	while (1) {
		std::string servIn = sock.recv();
		{
			std::lock_guard<std::recursive_mutex> vector_lock(readLock);
			text = text + servIn + "\n";
		}
		screenRefresher();
	}
}

void writeServer(MySocket sock) {
	while (1) {
		char input;
		std::cin >> input;

		if (input == '\n') {
			if (buffer.size() == 1) {
				if (buffer[0] == 'q' || buffer[0] == 'Q') {
					sock.send("::end::");
					break;
				}
			}
			else if (buffer.size() != 0) {
				sock.send(buffer);
			}
		}
		else {
			buffer.push_back(getch());
		}

		screenRefresher();
	}
}
void screenRefresher() {
	system("clear");
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n";
	std::cout << text;
	std::cout << std::endl;
	std::cout << "send message : ";
	std::cout << buffer;
}

void changeInputType(bool type) {
	struct termios t;

	if (type) {
		tcgetattr(STDIN_FILENO, &t);
		t.c_lflag &= ~ICANON;
		tcsetattr(STDIN_FILENO, TCSANOW, &t);
	}
	else {
		tcgetattr(STDIN_FILENO, &t);
		t.c_lflag |= ICANON;
		tcsetattr(STDIN_FILENO, TCSANOW, &t);
	}
}