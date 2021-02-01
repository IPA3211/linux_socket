#include "../headers/mySocket.h"
#include <thread>
#include <termios.h>
#include <mutex>

void readServer(MySocket sock);
void writeServer(MySocket sock);
void screenRefresher();
int getch(void);

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

	std::thread read(readServer, servSock);
	std::thread write(writeServer, servSock);
	std::thread screen(screenRefresher);

	read.join();
	write.~thread();
	screen.~thread();

	servSock.close();
}

void readServer(MySocket sock) {
	while (1) {
		std::string servIn = sock.recv();
		{
			std::lock_guard<std::recursive_mutex> vector_lock(readLock);
			text = text + servIn + "\n";
		}
		system("clear");
	}
}

void writeServer(MySocket sock) {
	while (1) {
		int input = getch();
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
	}
}
void screenRefresher() {
	while (1) {
		system("clear");
		std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n";
		std::cout << text;
		std::cout << std::endl;
		std::cout << "send message : ";
		std::cout << buffer;
	}
}

int getch(void) {
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}