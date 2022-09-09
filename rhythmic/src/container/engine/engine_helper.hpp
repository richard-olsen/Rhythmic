#ifndef RHYTHMIC_ENGINE_HELPER_H
#define RHYTHMIC_ENGINE_HELPER_H

#include "engine.hpp"

namespace Rhythmic 
{

	void SerializeFret5(Engine engine, std::ofstream *ofile);
	void DeserializeFret5(EngineFret5Variables *vars, std::vector<unsigned char> data, int *index);

	void SerializeDrums(Engine engine, std::ofstream *ofile);
	void DeserializeDrums(EngineDrumsVariables *vars, std::vector<unsigned char> data, int *index);

}

#endif