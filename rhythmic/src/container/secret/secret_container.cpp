#include "secret_container.hpp"
#include "secret.hpp"

#include "../../engine/include/wge/core/logging.hpp"

namespace Rhythmic
{
	namespace SecretContainer
	{
		std::vector<Secret*> g_secrets;
		std::vector<Secret*>* GetSecrets() { return &g_secrets; }

		void CreateSecret(Secret *secret)
		{
			for (Secret *m_secret : g_secrets)
			{
				if (m_secret->m_type == secret->m_type)
				{
					LOG_INFO("Two secrets with the same type cannot be created!")
					return;
				}
			}

			g_secrets.push_back(secret);
		}

		Secret *GetSecret(SecretType type)
		{
			for (Secret *secret : g_secrets)
			{
				if (secret->m_type == type) return secret;
			}
			return 0;
		}

		bool IsSecretUnlocked(SecretType type)
		{
			for (Secret* secret : g_secrets)
			{
				if (secret->m_type == type && secret->m_isUnlocked)
				{
					return true;
				}
			}
			return false;
		}

		void InitializeSecrets()
		{
			// g = 9, r = 10, y = 11, b = 12, o = 13			- SINGLE NOTES
			// g = 512, r = 1024, y = 2048, b = 4096, o = 8192  - CHORDS

			uint64_t G = 1;
			uint64_t R = 2;
			uint64_t Y = 4;
			uint64_t B = 8;
			uint64_t O = 16;

			// OBOYOBOY
			std::vector<InputGameField> infSpeed{ O, B, O, Y, O, B, O, Y };
			CreateSecret(new Secret(INFINITE_NOTESPEED, infSpeed));

			//YOBOYOBO
			std::vector<InputGameField> negSpeed{ Y, O, B, O, Y, O, B, O};
			CreateSecret(new Secret(NEGATIVE_NOTESPEED, negSpeed));

			//// Closer chorus
			//std::vector<InputGameField> infEngine{3584, 3584, 3584, 3584, 3584, 
			//6656, 6656, 6656, 6656, 6656, 6656, 6656,
			//11264, 11264, 11264, 11264};
			//CreateSecret(new Secret(INFINITE_ENGINE, infEngine));

			//// N
			//std::vector<InputGameField> negEngine{G + R + Y + B + O, 
			//	R, Y,
			//G + R + Y + B + O };
			//CreateSecret(new Secret(NEGATIVE_ENGINE, negEngine));
		}

	}
}