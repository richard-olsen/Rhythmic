#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include "../events/event_system.hpp"
#include "../util/singleton.hpp"

namespace Rhythmic
{
	void InitializeBaseNet();
	void DestroyBaseNet();

	class Network : public Singleton<Network>
	{
		friend class Singleton<Network>;
	public:
		//void ShowNetwork();

		//void EstablishConnection(const std::string &ip);
		//void CloseConnection();
	private:
		Network();
		~Network();

		//NetSocketDatagram m_socket;
	};
}

#endif

