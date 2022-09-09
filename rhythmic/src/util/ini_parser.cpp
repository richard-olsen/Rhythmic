#include "ini_parser.hpp"

#include <fstream>
#include <regex>
#include "misc.hpp"

namespace Rhythmic
{
	bool IniContainsSection(const IniFile &file, const std::string &section)
	{
		auto it = file.m_sections.find(section);
		if (it != file.m_sections.end())
			return true;
		return false;
	}
	bool IniContainsKey(const IniMap &map, std::string key)
	{
		Util::ToLower(key);
		auto it = map.find(key);
		if (it != map.end())
			return true;
		return false;
	}
	std::string IniGetValue(const IniMap &map, const std::string &key, const std::string &defValue)
	{
		if (IniContainsKey(map, key))
			return map.at(key);
		return defValue;
	}

	/*
	 * Implementation based on boost's implementation
	 */
	IniSuccess IniRead(IniFile &file, const std::string &filename)
	{
		std::ifstream input(filename);
		
		IniSuccess success;
		success.errorMessage = "";
		success.success = 0;

		if (!input.is_open())
		{
			success.errorMessage = std::string("Missing Ini: ") + filename;
			success.success = INI_NOT_FOUND;
			return success;
		}

		IniMap *currentMap = &file.m_global;

		const char lbracket = input.widen('[');
		const char rbracket = input.widen(']');
		const char semicolon = input.widen(';');
		const char hashtag = input.widen('#');
		const char equal = input.widen('=');


		while (input.good())
		{
			std::string line;
			std::getline(input, line);

			Util::Trim(line);
			if (line.empty())
				continue;

			if (!(line[0] == semicolon || line[0] == hashtag))
			{
				if (line[0] == lbracket)
				{
					size_t end = line.find(rbracket);
					if (end != std::string::npos)
					{
						std::string sectionName = line.substr(1, end - 1);
						Util::ToLower(sectionName);
						file.m_sections.insert(std::pair<std::string, IniMap>(sectionName, IniMap()));
						currentMap = &file.m_sections[sectionName];
					}
					else
					{
						input.close();
						success.errorMessage = std::string(line) + ": Missing ]";
						success.success = INI_INVALID_SYNTAX_SECTION;
						return success;
					}
				}
				else
				{
					size_t equalLocation = line.find(equal);
					if (equalLocation == std::string::npos)
					{
						input.close();
						success.errorMessage = std::string(line) + ": Expected =";
						success.success = INI_INVALID_SYNTAX_EXPECTED_EQUALS;
						return success;
					}


					std::string key = line.substr(0, equalLocation);
					Util::Trim(key);
					Util::ToLower(key);

					std::string data = line.substr(equalLocation + 1);
					Util::Trim(data);

					(*currentMap)[key] = data;
				}
			}
		}

		input.close();
		return success;
	}

	void WriteKeyAndValue(std::ofstream &stream, const std::string &key, const std::string &value)
	{
		std::string line = key + " = " + value + "\n";
		stream.write(line.c_str(), line.size());
	}

	int IniWrite(const IniFile &file, const std::string &filename)
	{
		std::ofstream output = std::ofstream(filename);
		output.clear();

		for (auto it = file.m_global.begin(); it != file.m_global.end(); it++)
		{
			WriteKeyAndValue(output, it->first, it->second);
		}

		for (auto section = file.m_sections.begin(); section != file.m_sections.end(); section++)
		{
			std::string sectionName = std::string("[") + section->first + "]\n";
			output.write(sectionName.c_str(), sectionName.size());

			for (auto val = section->second.begin(); val != section->second.end(); val++)
			{
				WriteKeyAndValue(output, val->first, val->second);
			}
		}

		output.close();
		return 0;
	}
}