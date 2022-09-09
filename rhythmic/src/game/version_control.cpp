#include "version_control.hpp"

#include "../util/version.hpp"
#include "../../engine/include/wge/core/logging.hpp"

#include "../util/json.hpp"
#include "../util/http/httplib.h"

#if !RHYTHMIC_TESTER_BUILD
#define API_PATH "/rhythmic/api/game"
#else
#define API_PATH "/rhythmic/api/game?tester"
#endif

namespace Rhythmic
{

	void VersionControl::ReadVersion()
	{
		if (m_correctVersion.empty())
		{
			httplib::Client cli("warmful.xyz");
			cli.set_follow_location(true);

			auto res = cli.Get(API_PATH);

			if (res && res->status == 200)
			{
				m_correctVersion = res->body;

				// Parse JSON
				using nlohmann::json;

				json jRoot = json::parse(m_correctVersion);
				json jVersion = jRoot.at("version");

				int id = jVersion.at("id");
				std::string overview = jVersion.at("overview");
				std::string changelog = jVersion.at("changelog");
				std::string version = jVersion.at("version");
				std::string url = jVersion.at("url");
				int date = jVersion.at("date");

				// Set struct fields
				m_externalVersion.id = id;
				m_externalVersion.overview = overview;
				m_externalVersion.changelog = changelog;
				m_externalVersion.version = version;
				m_externalVersion.url = url;
				m_externalVersion.date = date;
			}
		}
	}
	bool VersionControl::IsLatestVersion()
	{
		return RHYTHMIC_VERSION_ID >= m_externalVersion.id;
	}

	VersionControl::ExternalVersion VersionControl::GetCorrectVersion() // correctVersion is raw JSON data so it returns externalVersion as it contains all the actual data from the JSON
	{
		return m_externalVersion; 
	}

	VersionControl::VersionControl() :
		m_actualVersion(GetRhythmicVersionStringShortened()),
		m_correctVersion()
	{ }
}
