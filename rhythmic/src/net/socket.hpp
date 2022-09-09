#ifndef SOCKET_HPP_
#define SOCKET_HPP_


#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

namespace Rhythmic
{
	class NetSocketDatagram
	{
	public:
		NetSocketDatagram();
		~NetSocketDatagram();

		void Bind(const char *ip, unsigned short port);
		void Connect(const char *ip, unsigned short port);

		void Send(const char *data, unsigned int size);

		void Receive(char data[1024], int &size);
	private:
		unsigned int m_socket;
		//sockaddr_in m_address;
	};
}

#endif

