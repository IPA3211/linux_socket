#pragma once

#include <iostream>

#ifdef __linux__

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#endif

#ifdef _WIN32
#include <WinSock2.h>
#endif

#define TCP 0
#define UDP 1

#define BUFFER_SIZE 1024

class SockAddrSet
{
public:
	sockaddr_in _addr;
	socklen_t _size;

	SockAddrSet(const std::string &ip, short port);
	SockAddrSet(short port);
	SockAddrSet();
	~SockAddrSet();
};

SockAddrSet::SockAddrSet(const std::string &ip, short port)
{
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	_addr.sin_port = htons(port);

	_size = sizeof(_addr);
}

SockAddrSet::SockAddrSet(short port) {
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);

	_size = sizeof(_addr);
}

SockAddrSet::SockAddrSet() {	_size = sizeof(_addr);}

SockAddrSet::~SockAddrSet()
{
	
}

class MySocket
{
public:
	bool debugMode = false;

	MySocket(const int &type);
	MySocket(int sock, const int &type, sockaddr_in addr);

	MySocket& operator=(MySocket& ref);

	int bind(const short &port);
	int connect(std::string ip, const short &port);
	int listen(int waiting);
	MySocket accept();

	int send(std::string msg);
	int sendTo(std::string msg, const SockAddrSet& to);
	std::string recv();
	std::string recvFrom(SockAddrSet& from);

	int close();

	void getSocketInfo(int& _sock, int& _port, int& _type, sockaddr_in& addr);

	static void errorHandler(std::string message);
	
	~MySocket();

private:
	int _type = 0;
	int _sock = 0;
	short _port = 0;
	std::string _ip = "";
	sockaddr_in _addr = {};

	bool isConnected = false;
};

MySocket::MySocket(const int &type)
{
	_type = type;
	switch (type)
	{
	case TCP :
		_sock = socket(PF_INET, SOCK_STREAM, 0);
		break;
	case UDP :
		_sock = socket(PF_INET, SOCK_DGRAM, 0);
	default:
		break;
	}
}

MySocket::MySocket(int sock, const int &type, sockaddr_in addr) {
	_sock = sock;
	_type = type;
	_addr = addr;
}

MySocket& MySocket::operator=(MySocket& ref) {
	_type = ref._type;
	_sock = ref._sock;
	_port = ref._port;
	_ip = ref._ip;
	_addr = ref._addr;
	return *this;
}

int MySocket::bind(const short &port) {
	_port = port;
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);

	int temp = ::bind(_sock, (sockaddr *)&_addr, sizeof(_addr));

	if (debugMode) {
		if (temp == -1) {
			errorHandler("bind() error");
		}
	}

	return temp;
}

int MySocket::connect(std::string address, const short &port) {
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = inet_addr(address.c_str());
	_addr.sin_port = htons(port);

	int temp = ::connect(_sock, (struct sockaddr*)&_addr, sizeof(_addr));

	if (debugMode) {
		if (temp == -1) {
			errorHandler("connect() error");
		}
	}
	
	if(temp != -1)
		isConnected = true;

	return temp;
}

int MySocket::listen(int wating) {
	int temp = ::listen(_sock, wating);

	if (debugMode) {
		if (temp == -1) {
			errorHandler("listen() error");
		}
	}

	return temp;
}

MySocket MySocket::accept() {
	sockaddr_in clntAddr = {};
	socklen_t clntAddrSize;

	clntAddrSize = sizeof(clntAddr);

	int clntSock = ::accept(_sock, (sockaddr *)&clntAddr, &clntAddrSize);

	if (debugMode) {
		if (clntSock == -1) {
			errorHandler("accept() error");
		}
	}

	MySocket tempSock(clntSock, _type, clntAddr);

	return tempSock;
}

int MySocket::send(std::string msg) {
	return write(_sock, msg.c_str(), msg.size());
}

int MySocket::sendTo(std::string msg, const SockAddrSet& to) {
	return ::sendto(_sock, msg.c_str(), msg.size(), 0,
		(struct sockaddr*)&to, sizeof(to));
}

std::string MySocket::recv() {
	char readBuffer[BUFFER_SIZE];
	int recvSize;
	std::string data;
	while (1) {
		recvSize = read(_sock, readBuffer, BUFFER_SIZE - 1);

		readBuffer[recvSize] = 0;

		data = data + readBuffer;

		if (recvSize < BUFFER_SIZE - 1) {
			break;
		}
	}

	data.push_back(0);

	return data;
}

std::string MySocket::recvFrom(SockAddrSet& from) {
	char readBuffer[BUFFER_SIZE];
	int recvSize;
	std::string data;
	while (1) {
		recvSize = ::recvfrom(_sock, readBuffer, BUFFER_SIZE - 1, 0,
						 (struct sockaddr *)&from._addr, &from._size);

		readBuffer[recvSize] = 0;

		data = data + readBuffer;

		if (recvSize < BUFFER_SIZE - 1) {
			break;
		}
	}

	data.push_back(0);

	return data;
}

int MySocket::close() {
	::close(_sock);
	return 0;
}

MySocket::~MySocket()
{
	this->close();
}

void MySocket::errorHandler(std::string message) {
	std::cout << message << std::endl;
	exit(1);
}
