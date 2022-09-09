#ifndef RHYTHMIC_FRETBOARD_H
#define RHYTHMIC_FRETBOARD_H

#include <string>

namespace Rhythmic
{
	typedef int FretboardHandle;

	constexpr FretboardHandle FRETBOARD_DEFAULT = 0;
	constexpr char *FRETBOARD_DEFAULT_FILE = "/default.png";

	struct Fretboard
	{
		std::string m_name;
		std::string m_fileName;
	};
}

#endif