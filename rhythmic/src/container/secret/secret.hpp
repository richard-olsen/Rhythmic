#ifndef RHYTHMIC_SECRET_HPP_
#define RHYTHMIC_SECRET_HPP_

#include "../../input/input_game.hpp"

#include <vector>
#include <functional>

namespace Rhythmic
{

	enum SecretType
	{
		INFINITE_NOTESPEED = 1,
		NEGATIVE_NOTESPEED = 2,
		INFINITE_ENGINE = 3,
		NEGATIVE_ENGINE = 4,
	};

	class Secret
	{
	public:
		Secret(SecretType type, std::vector<InputGameField> buttonSequence, std::function<void(void)> onUnlock = 0);

		void Unlock();

		std::vector<InputGameField> m_buttonSequence;
		SecretType m_type;
		std::function<void(void)> m_onUnlock;
		bool m_isUnlocked = false;
	private:
	};

}
#endif