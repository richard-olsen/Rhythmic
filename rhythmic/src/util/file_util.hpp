#ifndef RHYTHMIC_FILE_UTIL_H_
#define RHYTHMIC_FILE_UTIL_H_

// FUCK UNICODE AND THEIR DAMN BOM's
// NO WONDER SOME FILES FAIL TO LOAD

#include <locale>
#include <fstream>
#include <string>

#include <filesystem>

namespace Rhythmic
{
	enum class FileEncoding
	{
		FILE_ENC_UTF8,
		FILE_ENC_UTF16_LE,
		FILE_ENC_UTF16_BE,
		FILE_ENC_UTF32_LE,
		FILE_ENC_UTF32_BE,
		FILE_ENC_UNKNOWN
	};

	namespace FileUtil
	{
		void ReadFileToUTF8(const std::filesystem::path &fileName, std::string &out);

		FileEncoding CheckBOM(std::ifstream &stream, bool *hasBOM = 0);
	}
}

#endif

