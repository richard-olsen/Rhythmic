//#include "socket.hpp"
//
//#ifdef WIN32
//// Include Windows libs
//
//#include <WinSock2.h>
//#include <WS2tcpip.h>
//
//#else
//// Include Linux libs
//#endif
//
//namespace Rhythmic
//{
//#ifdef WIN32
//	// Windows implementation
//
//	NetSocketDatagram::NetSocketDatagram()
//	{
//		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
//	}
//	NetSocketDatagram::~NetSocketDatagram()
//	{
//		closesocket(m_socket);
//	}
//
//	void NetSocketDatagram::Bind(const char *ip, unsigned short port)
//	{
//		Connect(ip, port);
//		int result = bind(m_socket, (sockaddr*)&m_address, sizeof(sockaddr));
//	}
//	void NetSocketDatagram::Connect(const char *ip, unsigned short port)
//	{
//		m_address.sin_family = AF_INET;
//		m_address.sin_port = htons(port); // Little to big endian conversion
//		inet_pton(AF_INET, ip, &m_address.sin_addr); // Convert from string to byte array
//	}
//
//	void NetSocketDatagram::Send(const char *data, unsigned int size)
//	{
//		sendto(m_socket, data, size, 0, (sockaddr*)&m_address, sizeof(sockaddr));
//	}
//
//	void NetSocketDatagram::Receive(char data[1024], int &size)
//	{
//		sockaddr_in sender;
//		int senderSize = sizeof(sender);
//		ZeroMemory(&sender, senderSize);
//
//		size = recvfrom(m_socket, data, 1024, 0, (sockaddr*)&sender, &senderSize);
//	}
//
//#else
//	// Linux implementation
//#endif
//}