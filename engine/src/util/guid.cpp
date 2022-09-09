#include <wge/util/guid.hpp>

#include <memory>
#include <algorithm>
#include <cstring>

void generate(char guid[16]);

// Windows implementation
#ifdef WIN32
#include <objbase.h>

void generate(char guid[16])
{
	GUID nativeGuid;

	HRESULT hr = CoCreateGuid(&nativeGuid);

	memcpy(guid, &nativeGuid, GUID_CHAR_COUNT);
}

#else
// Linux implementation
#include <uuid/uuid.h>

void generate(char guid[16])
{
	uuid_t uuid;
	
	uuid_generate(uuid);

	memcpy(guid, uuid, GUID_CHAR_COUNT);
}


#endif

/*
Converts val to a hex value
val is capped between 0 and 15
*/
char NibbleToHex(char val)
{
	val &= 0x0F;

	if (val < 10)
		return '0' + val;
	else
		return 'A' + (val - 10);
}

char NibbleFromHex(char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';

	if (hex >= 'a' && hex <= 'f')
		return hex - 'a' + 10;

	if (hex >= 'A' && hex <= 'F')
		return hex - 'A' + 10;

	return 0;
}

char FromHex(char hiNibble, char loNibble)
{
	char output = 0;

	output |= (NibbleFromHex(hiNibble) << 4);
	output |= (NibbleFromHex(loNibble));

	return output;
}

void PushHex(std::string &string, char val)
{
	char hiNibble = NibbleToHex((val >> 4) & 0x0F);
	char loNibble = NibbleToHex(val & 0x0F);

	string.push_back(hiNibble);
	string.push_back(loNibble);
}

namespace WGE
{
	namespace Util
	{
		GUID::GUID(bool autoGenerate)
		{
			if (autoGenerate)
				Generate();
			else
				memset(m_guid, 0, GUID_CHAR_COUNT);
		}
		GUID::GUID(const std::string &string)
		{
			size_t size = string.size();
			if (size != 36 && size != 32)
			{
				memset(m_guid, 0, GUID_CHAR_COUNT);
				return;
			}

			// Need to test if it's a valid guid (with or without -)
			auto invalidChar = string.end();
			if (size == 36)
				invalidChar = std::find_if_not(string.begin(), string.end(),
					[](char c)
					{
						return
							(c >= '0' && c <= '9') ||
							(c >= 'a' && c <= 'f') ||
							(c >= 'A' && c <= 'F') ||
							c == '-';
					});
			else
				invalidChar = std::find_if_not(string.begin(), string.end(),
					[](char c)
					{
						return
							(c >= '0' && c <= '9') ||
							(c >= 'a' && c <= 'f') ||
							(c >= 'A' && c <= 'F');
					});

			if (invalidChar != string.end())
			{
				memset(m_guid, 0, GUID_CHAR_COUNT);
				return;
			}

			if (size == 36) // Contains formatting
			{
				m_guid[0] = FromHex(string[0], string[1]);
				m_guid[1] = FromHex(string[2], string[3]);
				m_guid[2] = FromHex(string[4], string[5]);
				m_guid[3] = FromHex(string[6], string[7]);

				m_guid[4] = FromHex(string[9], string[10]);
				m_guid[5] = FromHex(string[11], string[12]);

				m_guid[6] = FromHex(string[14], string[15]);
				m_guid[7] = FromHex(string[16], string[17]);

				m_guid[8] = FromHex(string[19], string[20]);
				m_guid[9] = FromHex(string[21], string[22]);

				m_guid[10] = FromHex(string[24], string[25]);
				m_guid[11] = FromHex(string[26], string[27]);
				m_guid[12] = FromHex(string[28], string[29]);
				m_guid[13] = FromHex(string[30], string[31]);
				m_guid[14] = FromHex(string[32], string[33]);
				m_guid[15] = FromHex(string[34], string[35]);
			}
			else // Doesn't contain formatting
			{
				for (size_t i = 0; i < GUID_CHAR_COUNT; i++)
				{
					m_guid[i] = FromHex(string[i * 2], string[i * 2 + 1]);
				}
			}
		}

		void GUID::Generate()
		{
			generate(m_guid);
		}

		std::string GUID::ToString() const
		{
			std::string output;
			
			PushHex(output, m_guid[0]);
			PushHex(output, m_guid[1]);
			PushHex(output, m_guid[2]);
			PushHex(output, m_guid[3]);
			output.push_back('-');
			PushHex(output, m_guid[4]);
			PushHex(output, m_guid[5]);
			output.push_back('-');
			PushHex(output, m_guid[6]);
			PushHex(output, m_guid[7]);
			output.push_back('-');
			PushHex(output, m_guid[8]);
			PushHex(output, m_guid[9]);
			output.push_back('-');
			PushHex(output, m_guid[10]);
			PushHex(output, m_guid[11]);
			PushHex(output, m_guid[12]);
			PushHex(output, m_guid[13]);
			PushHex(output, m_guid[14]);
			PushHex(output, m_guid[15]);

			return output;
		}

		bool GUID::operator==(const GUID &rhs) const
		{
			return memcmp(m_guid, rhs.m_guid, GUID_CHAR_COUNT) == 0;
		}
		bool GUID::operator!=(const GUID &rhs) const
		{
			return memcmp(m_guid, rhs.m_guid, GUID_CHAR_COUNT) != 0;
		}

		bool GUID::operator<(const GUID &rhs) const
		{
			return memcmp(m_guid, rhs.m_guid, GUID_CHAR_COUNT) < 0;
		}

		GUID &GUID::operator=(const GUID &rhs)
		{
			memcpy(m_guid, rhs.m_guid, GUID_CHAR_COUNT);
			return *this;
		}

		GUID::operator std::string() const
		{
			return ToString();
		}
	}
}
