#include "file_util.hpp"

#include <sstream>
#include <fstream>

#include "utf8.h"

namespace Rhythmic
{
	namespace FileUtil
	{
		void ReadFileToUTF8(const std::filesystem::path &fileName, std::string &out)
		{
			int fileSize = std::filesystem::file_size(fileName);

			out.clear();
			std::ifstream stream = std::ifstream(fileName.string(), std::ios::binary);
			if (!stream.is_open())
				return;
			if (stream.eof())
				return;
			
			bool hasBOM = false;
			FileEncoding encoding = CheckBOM(stream, &hasBOM);

			if (encoding == FileEncoding::FILE_ENC_UTF8)
			{
				if (hasBOM)
					fileSize -= 3;

				out.resize(fileSize);

				stream.read(&out[0], fileSize);
			}
			else if (encoding == FileEncoding::FILE_ENC_UTF16_LE)
			{
				fileSize = (fileSize - 2) / 2;

				std::u16string u16string;
				u16string.resize(fileSize);

				stream.read((char *)&u16string[0], u16string.size() * 2);
				std::string u8string;

				utf8::utf16to8(u16string.begin(), u16string.end(), std::back_inserter(out));
			}
		}
		FileEncoding CheckBOM(std::ifstream &stream, bool *hasBOM)
		{
			int bom1 = 0;
			int bom2 = 0;
			int bom3 = 0;
			bom1 = stream.get();
			bom2 = stream.get();

			std::stringstream ss;

			int encoding = 0; // 0 for UTF8

			if (bom1 == 0xFF && bom2 == 0xFE)
			{
				if (hasBOM)
					*hasBOM = true;
				return FileEncoding::FILE_ENC_UTF16_LE;
			}
			//else if (bom1 == 0xFE && bom2 == 0xFF)
			//{
			//	// UTF-16 encoded BE
			//	encoding = 2;
			//}
			else
			{
				bom3 = stream.get();
				if (bom1 == 0xEF && bom2 == 0xBB && bom3 == 0xBF)
				{
					if (hasBOM)
						*hasBOM = true;
					return FileEncoding::FILE_ENC_UTF8;
				}
				else
				{
					stream.seekg(0, std::ios::beg); // Put the stream back at the beginning

					if (hasBOM)
						*hasBOM = false;
					return FileEncoding::FILE_ENC_UTF8;
				}
			}

			return FileEncoding::FILE_ENC_UNKNOWN;
		}
	}
}
