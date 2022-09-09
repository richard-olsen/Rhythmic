#ifndef RHYTHMIC_ENGINE_CONTAINER_H
#define RHYTHMIC_ENGINE_CONTAINER_H

#include <map>
#include "engine.hpp"

namespace Rhythmic 
{
	namespace EngineContainer 
	{
		std::map<WGE::Util::GUID, Engine> *GetEngines();
		Engine *GetCurrentEngine();

		Engine *GetEngine(WGE::Util::GUID &guid);

		void SetCurrentEngine(Engine &engine);

		bool GetEngineFromFile(Engine *engine, std::string filename);
		void LoadCustomEngines();

		void DeleteEngineFile(Engine engine);
		void SaveEngineToFile(Engine *engine);

		void Initialize();
	}
}
#endif