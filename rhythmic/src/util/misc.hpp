#ifndef MISC_H_
#define MISC_H_

#include <string>
#include <vector>

namespace Rhythmic
{
	namespace Util
	{
		/*
		Initializes a few variables so they don't have to be repeatedly fetched upon request
		*/
		void Initialize();

		const std::string &GetExecutablePath();
		void SetExecutablePath(const std::string &str);

		double GetTime();

		template<typename T>
		void Serialize_LE(T in, unsigned char *out)
		{
			size_t size = sizeof(T);
			for (int i = 0; i < size; i++)
				out[i] = (in >> (i * 8)) & 0xFF;
		}
		template<typename T>
		void Serialize_BE(T in, unsigned char *out)
		{
			size_t size = sizeof(T);
			for (int i = 0; i < size; i++)
				out[i] = (in >> ((size - (i + 1)) * 8)) & 0xFF;
		}

		template<typename T>
		T Deserialize_LE(unsigned char *in)
		{
			size_t size = sizeof(T);
			T val = 0;
			for (int i = 0; i < size; i++)
				val |= (in[i] << (i * 8));
			return val;
		}

		template<typename T>
		T Deserialize_BE(unsigned char *in)
		{
			size_t size = sizeof(T);
			T val = 0;
			for (int i = 0; i < size; i++)
				val |= (in[i] << ((size - (i + 1)) * 8));
			return val;
		}

		std::string RemoveQuotes(const std::string &string);
		void Trim(std::string &string);
		void Split(const std::string &string, const std::string &delimiter, std::vector<std::string> &tokens);
		void ToLower(std::string &string);
		int compareStringCaseInsensitive(std::string string1, std::string string2);

		/*
		Called to request the main loop to close the game (just a more proper way instead of calling exit(0), and will save things that are neccessary that haven't been saved yet)
		failed -> Defaults to false. If the method is called with failed true, the game changes the close state to false
		*/
		void RequestGameToClose(bool failed = false);
		bool HasGameRequestedToClose();

		const char *GetKeyName(int value);
	} // namespace Util
}

#endif
