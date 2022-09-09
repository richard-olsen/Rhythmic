#include "engine_container.hpp"
#include "engine.hpp"
#include "engine_helper.hpp"

#include <vector>

#include <filesystem>

#include "../../util/misc.hpp"
#include "../../util/settings.hpp"

#include <wge/core/logging.hpp>
#include <wge/util/guid.hpp>

#include <fstream>

#define ENGINE_VERSION 1

struct Engine_FileHeader // 32 bytes (20 bytes used - 12 overhead)
{
	unsigned int version; // 4 bytes
	WGE::Util::GUID guid; // 16 bytes
};

namespace Rhythmic 
{
	namespace EngineContainer 
	{
		Engine rhythmicEngine;

		std::map<WGE::Util::GUID, Engine> g_engines;
		std::map<WGE::Util::GUID, Engine> *GetEngines() { return &g_engines; }

		WGE::Util::GUID g_currentEngine;
		Engine *GetCurrentEngine() { return &g_engines[g_currentEngine]; }

		Engine *GetEngine(WGE::Util::GUID &guid)
		{
			auto it = g_engines.find(guid); // Try to find GUID in map

			if (it != g_engines.end()) // Found guid key if not at the end
			{
				return &it->second;
			}
			else return 0;
		}

		void InsertEngine(Engine engine)
		{
			if (engine.m_guid != 0) // If the GUID is not null, it is probably a custom engine.
			{
				g_engines.insert(std::make_pair(engine.m_guid, engine));
			}
			else // Create a new GUID and auto generate it if it is not set
			{
				WGE::Util::GUID guid = WGE::Util::GUID(true);

				engine.m_guid = guid;
				g_engines.insert(std::make_pair(guid, engine));
			}
		}

		void SetCurrentEngine(Engine &engine) {
			g_currentEngine = engine.m_guid;

			SettingValue value;
			value.type = SETTING_GAME_ENGINE;
			value.m_sValue = engine.m_guid; // Needs to be GUID but GUIDs aren't being parsed from strings right.
			SettingsSet(value);

			SettingsSave();
		}

		bool GetEngineFromFile(Engine *engine, std::string filename) {
			std::ifstream in(Util::GetExecutablePath() + "/custom/engines/" + filename, std::ios::binary); // Input stream

			if (!in.is_open()) return false;

			EngineFret5Variables fret5vars;
			EngineDrumsVariables drumVars;
			EngineKeysVariables keysVars;
			std::string name;
			std::string author;
			int version;
			WGE::Util::GUID guid;

			int index = 0;

			in.seekg(0, in.end);
			size_t size = in.tellg();
			in.seekg(0, in.beg);

			std::vector<unsigned char> data;
			data.resize(size);

			in.read((char*)&data[0], size);
			
			// Read file header
			version = Util::Deserialize_LE<int>(&data[index]);
			index += 4;
			guid = WGE::Util::GUID(std::string((char *)&data[index]));
			index += 36;

			// Overhead
			index += 16;

			DeserializeFret5(&fret5vars, data, &index);
			DeserializeDrums(&drumVars, data, &index);

			if (version > ENGINE_VERSION)
			{
				return false;
			}

			if (version >= 2) { // Engine versioning
				
			}
			else if (version >= 3) {

			}

			name = std::string((char *)&data[index]);
			index += name.size() + 1;
			if (!(index <= size)) // Author is missing if this is true
			{
				author = std::string("Unknown");
			} else author = std::string((char*)&data[index]);

			engine->m_fret5 = fret5vars;
			engine->m_drums = drumVars;
			engine->m_keys = keysVars;
			engine->m_name = name;
			engine->m_author = author;
			engine->m_version = version;
			engine->m_isCustom = true;
			engine->m_filename = filename;
			engine->m_guid = guid;

			return true;
		}

		void LoadCustomEngines() {
			std::string dir = Util::GetExecutablePath() + "/custom/engines";

			if (!std::filesystem::exists(Util::GetExecutablePath() + "/custom/"))
				std::filesystem::create_directory(Util::GetExecutablePath() + "/custom/");
			if (!std::filesystem::exists(dir))
				std::filesystem::create_directory(dir);

			int index = 0;
			for (std::filesystem::directory_iterator it(dir); it != std::filesystem::directory_iterator(); it++)
			{
				std::filesystem::path file = it->path();
				if (std::filesystem::is_regular_file(file))
				{
					std::string ext = file.extension().string();
					if (Util::compareStringCaseInsensitive(ext, ".engine") == 0) // Found engine
					{
						std::string filename = file.filename().string();

						Engine customEngine;
						if (!GetEngineFromFile(&customEngine, filename)) { // Returns false if it failed to load
							LOG_INFO("Failed to load engine: {0}", filename);
						}
						else {
							LOG_INFO("Successfully loaded engine: {0}. Author: {1} Engine version: {2}", customEngine.m_name, customEngine.m_author, customEngine.m_version);
							if (customEngine.m_version < ENGINE_VERSION) // Rewrite the file if it is out of date
							{
								std::filesystem::remove(file);
								SaveEngineToFile(&customEngine);
							}
							InsertEngine(customEngine);
						}
						index++;
					}
				}
			}
		}

		void DeleteEngineFile(Engine engine)
		{
			std::string filename = engine.m_filename;

			if (filename == "")
			{
				std::string filename = engine.m_name + ".engine";
				Util::ToLower(filename);

				for (std::string::iterator it = filename.begin(); it != filename.end(); ++it)
				{
					if (*it == ' ')
					{
						*it = '_';
					}
				}
			}

			std::string dir = Util::GetExecutablePath() + "/custom/engines";

			// If the folders don't exist then the user has manually deleted them or something has gone horribly wrong
			// So return out of it.
			if (!std::filesystem::exists(Util::GetExecutablePath() + "/custom/"))
			{
				std::filesystem::create_directory(Util::GetExecutablePath() + "/custom/");
				return;
			}
			if (!std::filesystem::exists(dir))
			{
				std::filesystem::create_directory(dir);
				return;
			}

			for (std::filesystem::directory_iterator it(dir); it != std::filesystem::directory_iterator(); it++)
			{
				std::filesystem::path file = it->path();
				if (std::filesystem::is_regular_file(file))
				{
					std::string ext = file.extension().string();
					if (Util::compareStringCaseInsensitive(ext, ".engine") == 0) // Found engine
					{
						if (file.filename().string() == filename) // Engine matches one we want to delete
						{
							std::filesystem::remove(file);
							LOG_INFO("{0} deleted", filename)
							return;
						}
					}
				}
			}
		}

		void SaveEngineToFile(Engine *engine) {
			std::string filename = engine->m_name;
			Util::ToLower(filename);

			for (std::string::iterator it = filename.begin(); it != filename.end(); ++it) {
				if (*it == ' ') {
					*it = '_';
				}
			}

			engine->m_filename = filename + ".engine";

			std::ofstream out(Util::GetExecutablePath() + "/custom/engines/" + filename + ".engine", std::ios::out | std::ios::binary);
			out.clear();

			Engine_FileHeader headerData;
			headerData.version = ENGINE_VERSION; // 4 bytes
			headerData.guid = engine->m_guid; // 16 bytes

			std::string sGUID = headerData.guid.ToString();

			// version = 4
			// guid string = 36

			unsigned char headerOut[56];
			Util::Serialize_LE(headerData.version, &headerOut[0]);
			// headerOut[4];
			// 4 - 39 = guid;
			// GUID string size = 36 bytes but 16 extra bytes as overhead for things in the future.
			memcpy(&headerOut[4], sGUID.c_str(), 52);

			out.write((char *)headerOut, 56);

			SerializeFret5(*engine, &out);
			SerializeDrums(*engine, &out);
			out.write(engine->m_name.c_str() + '\0', engine->m_name.size() + 1);
			out.write(engine->m_author.c_str() + '\0', engine->m_author.size() + 1);
			out.close();

			LOG_INFO("Saved engine: " + std::string(engine->m_name));
		}

		void CreateEnginePresets() {
			EngineFret5Variables rb3Fret5 = { 100, false, 80, 100, 50, 30, 150, true };
			EngineDrumsVariables rb3Drums = { 100, 50, 100, 100 };
			EngineKeysVariables rb3Keys;
			Engine rb3 = { rb3Fret5, rb3Drums, rb3Keys, "Rock Band 3", "Rhythmic", ENGINE_VERSION };
			rb3.m_guid = WGE::Util::GUID("4c0b20aa-e752-42e0-a3d3-792a838a44f4");

			EngineFret5Variables gh1Fret5 = { 100, false, 70, 100, 40, 30, 100, true };
			EngineDrumsVariables gh1Drums;
			EngineKeysVariables gh1Keys;
			Engine gh1 = { gh1Fret5, gh1Drums, gh1Keys, "Guitar Hero 1", "Rhythmic", ENGINE_VERSION };
			gh1.m_guid = WGE::Util::GUID("82235944-637d-452c-a76d-bb43461af060");

			EngineFret5Variables gh3CHFret5 = { 0, true, 140, 140, 50, 50, 75, false };
			EngineDrumsVariables gh3CHDrums;
			EngineKeysVariables gh3CHKeys;
			Engine gh3CH = { gh3CHFret5, gh3CHDrums, gh3CHKeys, "GH3/CH", "Rhythmic", ENGINE_VERSION };
			gh3CH.m_guid = "8a679e00-345c-4f33-9a5d-117d3cb54c83";

			//InsertEngine(rb3);
			//InsertEngine(gh1);
			//InsertEngine(gh3CH);
		}

		void LoadEngineFromSettings() {
			SettingValue value;
			value.type = SETTING_GAME_ENGINE;
			SettingsGet(&value);

			std::string settingsString = value.m_sValue; // GUID

			if (settingsString == "") SetCurrentEngine(g_engines[0]);

			WGE::Util::GUID settingsGUID = WGE::Util::GUID(settingsString); // Create GUID object from string saved (A guid)
			
			auto it = g_engines.find(settingsGUID); // Find guid in map

			if (it != g_engines.end()) // Found GUID
			{
				SetCurrentEngine(it->second); // second = Engine
			}
			else
			{
				SetCurrentEngine(rhythmicEngine);
			}
		}

		void Initialize() {
			CreateEnginePresets();
			LoadCustomEngines();

			rhythmicEngine.m_name = "Rhythmic";
			rhythmicEngine.m_guid = "74131DF1-E374-4181-A49A-7Ef50A248A15";
			InsertEngine(rhythmicEngine);

			LoadEngineFromSettings();
		}

	}
}