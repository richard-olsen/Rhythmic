#ifndef RHYTHMIC_INI_PARSER_H_
#define RHYTHMIC_INI_PARSER_H_

#include <map>
#include <string>

namespace Rhythmic
{
	constexpr int INI_NOT_FOUND = -1;
	constexpr int INI_INVALID_SYNTAX_SECTION = -4;
	constexpr int INI_INVALID_SYNTAX_EXPECTED_EQUALS = -5;

	struct IniSuccess
	{
		int success;
		std::string errorMessage;
	};

	typedef std::map<std::string, std::string> IniMap;
	typedef std::pair<std::string, std::string> IniEntry;
	typedef std::pair<std::string, IniMap> IniSectionEntry;

	struct IniFile
	{
		std::string m_filename;

		IniMap m_global;
		
		std::map<std::string, IniMap> m_sections;
	};

	bool IniContainsSection(const IniFile &file, const std::string &section);
	bool IniContainsKey(const IniMap &map, std::string key);
	std::string IniGetValue(const IniMap &map, const std::string &key, const std::string &defValue = "");

	IniSuccess IniRead(IniFile &file, const std::string &filename);
	int IniWrite(const IniFile &file, const std::string &filename);
}

#endif

