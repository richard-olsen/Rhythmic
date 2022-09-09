#include "network.hpp"

#include "../util/dear_imgui/imgui.h"

#include <iostream>
#include <string>
#include <thread>

#ifdef WIN32
//#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif

#include "socket.hpp"

namespace Rhythmic
{
	/*
	Context info
	*/
	//WSAData g_netData;

	/*
	Socket info
	*/
	//bool g_serverStop;
	//std::shared_ptr<std::thread> g_serverThread;

	
	
	void InitializeBaseNet()
	{
		//WSAStartup(MAKEWORD(2, 2), &g_netData);
		
		//g_serverStop = true;
	}

	void DestroyBaseNet()
	{
		//WSACleanup();
	}

	//void Network::OnImGui(void *)
	//{
	//	static char buffer[33] = { 0 };

	//	ImGui::Begin("Network", &m_showWindow);

	//	ImGui::Text("Networking Menu");

	//	if (ImGui::Button("Host"))
	//	{
	//		/*socket_.async_receive_from(
	//			boost::asio::buffer(data_, max_length), sender_endpoint_,
	//			[this](boost::system::error_code ec, std::size_t bytes_recvd)
	//			{
	//				if (!ec && bytes_recvd > 0)
	//				{
	//					do_send(bytes_recvd);
	//				}
	//				else
	//				{
	//					do_receive();
	//				}
	//			});*/
	//		if (g_serverStop)
	//		{
	//			//g_serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

	//			//// Create a server hint structure for the server
	//			//sockaddr_in serverHint;
	//			//serverHint.sin_addr.S_un.S_addr = 0; // Us any IP address available on the machine
	//			//serverHint.sin_family = AF_INET; // Address format is IPv4
	//			//serverHint.sin_port = htons(30000); // Convert from little to big endian

	//			//// Try and bind the socket to the IP and port
	//			//if (bind(g_serverSocket, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	//			//{
	//			//	MessageBox(NULL, "Failed to create server socket!", "Error", MB_ICONERROR);
	//			//}
	//			/*g_serverSocket = new NetSocketDatagram();
	//			g_serverSocket->Bind(buffer, 30000);

	//			g_serverStop = false;

	//			g_serverThread.reset(new std::thread(HostThread));*/


	//		}
	//	}

	//	ImGui::InputText("ip", buffer, 32);
	//	if (ImGui::Button("Connect"))
	//	{
	//		/*char reply[max_length];
	//		udp::endpoint sender_endpoint;
	//		size_t reply_length = s.receive_from(
	//			boost::asio::buffer(reply, max_length), sender_endpoint);
	//		std::cout << "Reply is: ";
	//		std::cout.write(reply, reply_length);
	//		std::cout << "\n";*/

	//		sockaddr_in server;
	//		server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	//		server.sin_port = htons(30000); // Little to big endian conversion
	//		inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array
	//		

	//		// Socket creation, note that the socket type is datagram
	//		SOCKET out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//		

	//		// Write out to that socket
	//		std::string s("The Message :D");
	//		int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));

	//		if (sendOk == SOCKET_ERROR)
	//		{
	//			MessageBox(NULL, std::to_string(WSAGetLastError()).c_str(), "Error", MB_ICONERROR);
	//		}

	//		// Close the socket
	//		closesocket(out);

	//		/*NetSocketDatagram datagram;
	//		datagram.Connect(buffer, 30000);
	//		datagram.Send("The Message :D", 15);*/
	//	}

	//	if (ImGui::Button("Test..."))
	//	{

	//	}

	//	if (ImGui::Button("STOP"))
	//	{
	//	}

	//	ImGui::End();
	//}

	Network::Network()
	{
	}
	Network::~Network()
	{
	}
}