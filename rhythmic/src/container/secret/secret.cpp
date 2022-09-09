#include "secret.hpp"

#include "../../engine/include/wge/core/logging.hpp"

namespace Rhythmic
{
	Secret::Secret(SecretType type, std::vector<InputGameField> buttonSequence, std::function<void(void)> onUnlock)
	{
		for (InputGameField& field : buttonSequence)
		{
			// I need to manually bit shift chords and enter the values so it only bit shifts if it's a single note
			if (!field > 13)
			{
				field = (uint64_t)1 << field;
			}
		}

		m_type = type;
		m_buttonSequence = buttonSequence;
		m_onUnlock = onUnlock;
	}

	void Secret::Unlock()
	{
		if(m_onUnlock != 0)
			m_onUnlock();
		m_isUnlocked = true;
	}
}