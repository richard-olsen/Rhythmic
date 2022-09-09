#ifndef RHYTHMIC_VERSION_CONTROL_HPP_
#define RHYTHMIC_VERSION_CONTROL_HPP_

#include "../util/singleton.hpp"
#include <string>

namespace Rhythmic
{
	class VersionControl : public Singleton<VersionControl>
	{
		struct ExternalVersion
		{
			int id;
			std::string overview;
			std::string changelog;
			std::string version;
			std::string url;
			int date;
		};

		friend class Singleton<VersionControl>;
	public:
		void ReadVersion();
		bool IsLatestVersion();
		ExternalVersion GetCorrectVersion();
	private:
		VersionControl();

		std::string m_correctVersion;
		std::string m_actualVersion;

		ExternalVersion m_externalVersion;

	};
	
	
}

#endif

