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

class MySocket
{
public:
	MySocket(const int &type);
	MySocket(int sock, const int &type, sockaddr_in addr);
	MySocket& operator=(MySocket& ref);
	int bind(const short &port);
	int connect(std::string ip, const short &port);
	int listen(int waiting);
	MySocket accept();
	int send(std::string msg);
	int close();
	std::string recv();
	
	~MySocket();

private:
	int _type = 0;
	int _sock = 0;
	short _port = 0;
	std::string _ip = "";
	sockaddr_in _addr = {};
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

	return ::bind(_sock, (sockaddr *)&_addr, sizeof(_addr));
}

int MySocket::connect(std::string address, const short &port) {
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = inet_addr(address.c_str());
	_addr.sin_port = htons(port);

	return ::connect(_sock, (struct sockaddr*)&_addr, sizeof(_addr));
}

int MySocket::listen(int wating) {
	return ::listen(_sock, wating);
}

MySocket MySocket::accept() {
	sockaddr_in clntAddr = {};
	socklen_t clntAddrSize;

	clntAddrSize = sizeof(clntAddr);

	int clntSock = ::accept(_sock, (sockaddr *)&clntAddr, &clntAddrSize);

	MySocket tempSock(clntSock, _type, clntAddr);

	return tempSock;
}

int MySocket::send(std::string msg) {
	return write(_sock, msg.c_str(), msg.size());
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

int MySocket::close() {
	::close(_sock);
	return 0;
}

MySocket::~MySocket()
{
	this->close();
}
