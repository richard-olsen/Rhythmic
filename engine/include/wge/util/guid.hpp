#ifndef WGE_GUID_HPP_
#define WGE_GUID_HPP_

#define GUID_CHAR_COUNT 16

#include <string>

namespace WGE
{
	namespace Util
	{
		class GUID
		{
		public:
			GUID(bool generate = false); // Creates a NULL GUID, will generate one if generate is true
			GUID(const std::string &guid); // Creates a GUID from a valid GUID string. Will generate a GUID filled with 0 if string is an invalid guid

			// Generates a unique guid
			void Generate();

			// Converts the GUID into a string
			std::string ToString() const;

			bool operator==(const GUID &rhs) const;
			bool operator!=(const GUID &rhs) const;

			GUID &operator=(const GUID &rhs);

			bool operator<(const GUID &rhs) const;

			operator std::string() const;
		private:
			char m_guid[GUID_CHAR_COUNT];
		};
	}
}

#endif

