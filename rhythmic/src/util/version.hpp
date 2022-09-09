#ifndef RHYTHMIC_VERSION_H_
#define RHYTHMIC_VERSION_H_

#include <string>

/*
With every release/build/patch, increment this number by 1.
It is used to check if the build ID is lower than the web's version.
*/
#define RHYTHMIC_VERSION_ID 11

#define RHYTHMIC_VERSION_MAJOR 0
#define RHYTHMIC_VERSION_MINOR 11
#define RHYTHMIC_VERSION_PATCH 0

#define RHYTHMIC_VERSION_TYPE "Alpha"

#ifdef WGE_DEBUG
#define RHYTHMIC_VERSION_BUILD "Debug"
#else
#define RHYTHMIC_VERSION_BUILD "Release"
#endif

//#define RHYTHMIC_RELEASE_TYPE "Development"

//#define RHYTHMIC_RELEASE_TYPE "Experimental xx"
//#define RHYTHMIC_TESTER_BUILD 1
//#define RHYTHMIC_RELEASE_TYPE "Release Candidate xx"
#define RHYTHMIC_RELEASE_TYPE "Release"

inline std::string GetRhythmicVersionString()
{
	return 
		(std::string(RHYTHMIC_VERSION_TYPE) + " " + std::to_string(RHYTHMIC_VERSION_MAJOR) + "." + std::to_string(RHYTHMIC_VERSION_MINOR) + "." + std::to_string(RHYTHMIC_VERSION_PATCH) + " (Build: " + RHYTHMIC_VERSION_BUILD + ") " +
			+ RHYTHMIC_RELEASE_TYPE
			);
}

inline std::string GetRhythmicVersionStringShortened()
{
	return
		(std::string(RHYTHMIC_VERSION_TYPE) + " " +
			std::to_string(RHYTHMIC_VERSION_MAJOR) + "." + std::to_string(RHYTHMIC_VERSION_MINOR) + "." + std::to_string(RHYTHMIC_VERSION_PATCH)) + 
		std::string(RHYTHMIC_RELEASE_TYPE);
	// "VERSION_TYPE MAJOR.MINOR.PATCH TYPE"
	// Example:
	// "Alpha 0.11.0 Experimental 3"
	// "Alpha 0.13.6 Release"
	// "Alpha 0.12.0 Release Candidate 02"
}

#endif

