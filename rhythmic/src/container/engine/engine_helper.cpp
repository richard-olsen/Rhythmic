#include "engine_helper.hpp"
#include "engine.hpp"
#include "../../util/misc.hpp"

#include <fstream>

namespace Rhythmic 
{

	void SerializeFret5(Engine engine, std::ofstream *ofile) {

		unsigned char data[8];

		Util::Serialize_LE(engine.m_fret5.m_hopoFrontEnd, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_infiniteHOPOFrontEnd, &data[0]);
		ofile->write((const char*)data, 1);

		Util::Serialize_LE(engine.m_fret5.m_hopoTapRange, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_strumRange, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_strumLeniency, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_hopoLeniency, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_timeTapIgnoreOverstrum, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_fret5.m_antiGhost, &data[0]);
		ofile->write((const char*)data, 1);
	}

	void DeserializeFret5(EngineFret5Variables *vars, std::vector<unsigned char> data, int *index) { // 5 Fret serialization is 26 bytes
		int i = *index;

		vars->m_hopoFrontEnd = Util::Deserialize_LE<int>(&data[i]); // i = 0
		i += 4;
		vars->m_infiniteHOPOFrontEnd = Util::Deserialize_LE<bool>(&data[i]); // i = 4
		i += 1;
		vars->m_hopoTapRange = Util::Deserialize_LE<int>(&data[i]); // i = 5
		i += 4;
		vars->m_strumRange = Util::Deserialize_LE<int>(&data[i]); // i = 9
		i += 4;
		vars->m_strumLeniency = Util::Deserialize_LE<int>(&data[i]); // i = 13
		i += 4;
		vars->m_hopoLeniency = Util::Deserialize_LE<int>(&data[i]); // i = 17
		i += 4;
		vars->m_timeTapIgnoreOverstrum = Util::Deserialize_LE<int>(&data[i]); // i = 21
		i += 4;
		vars->m_antiGhost = Util::Deserialize_LE<bool>(&data[i]); // i = 25
		i += 1;
		// i = 26

		*index = i;
	}

	void SerializeDrums(Engine engine, std::ofstream *ofile) {
		unsigned char data[8];

		Util::Serialize_LE(engine.m_drums.m_rangeCatcher, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_drums.m_padActive, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_drums.m_chordTime, &data[0]);
		ofile->write((const char*)data, 4);

		Util::Serialize_LE(engine.m_drums.m_spActivationTime, &data[0]);
		ofile->write((const char*)data, 4);
	}

	void DeserializeDrums(EngineDrumsVariables *vars, std::vector<unsigned char> data, int *index) { // Drums serialization is 16 bytes
		int i = *index;

		vars->m_rangeCatcher = Util::Deserialize_LE<int>(&data[i]);
		i += 4;
		vars->m_padActive = Util::Deserialize_LE<int>(&data[i]);
		i += 4;
		vars->m_chordTime = Util::Deserialize_LE<int>(&data[i]);
		i += 4;
		vars->m_spActivationTime = Util::Deserialize_LE<int>(&data[i]);
		i += 4;

		*index = i;
	}

	void SerializeKeys(Engine engine, std::ofstream *ofile) {

	}

}