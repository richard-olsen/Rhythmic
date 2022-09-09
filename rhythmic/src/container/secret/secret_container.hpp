#ifndef RHYTHMIC_SECRET_CONTAINER_HPP
#define RHYTHMIC_SECRET_CONTAINER_HPP

#include "secret.hpp"

namespace Rhythmic
{
	namespace SecretContainer
	{
		std::vector<Secret*>* GetSecrets();

		void CreateSecret(Secret* secret);

		bool IsSecretUnlocked(SecretType type);

		void InitializeSecrets();

		Secret* GetSecret(SecretType type);

	}

}

#endif