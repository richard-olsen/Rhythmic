#ifndef CATCHER_MANAGER_H_
#define CATCHER_MANAGER_H_

#include "catcher.hpp"

#define CATCHER_GREEN (1 << Rhythmic::CATCHER_TYPE_GREEN)
#define CATCHER_RED (1 << Rhythmic::CATCHER_TYPE_RED)
#define CATCHER_YELLOW (1 << Rhythmic::CATCHER_TYPE_YELLOW)
#define CATCHER_BLUE (1 << Rhythmic::CATCHER_TYPE_BLUE)
#define CATCHER_ORANGE (1 << Rhythmic::CATCHER_TYPE_ORANGE)
#define CATCHER_WHITE (1 << Rhythmic::CATCHER_TYPE_WHITE)

#define CATCHER_POS_4_1 -0.8f
#define CATCHER_POS_4_2 -0.26f
#define CATCHER_POS_4_3 0.26f
#define CATCHER_POS_4_4 0.8f

#define CATCHER_POS_5_1 CATCHER_POS_4_1
#define CATCHER_POS_5_2 -0.4f
#define CATCHER_POS_5_3 0
#define CATCHER_POS_5_4 0.4f
#define CATCHER_POS_5_5 CATCHER_POS_4_4

#define CATCHER_POS_6_1 -0.4f
#define CATCHER_POS_6_2 0
#define CATCHER_POS_6_3 0.4f
#define CATCHER_POS_6_4 -0.4f
#define CATCHER_POS_6_5 0
#define CATCHER_POS_6_6 0.4f


namespace Rhythmic
{

	class CatcherManager 
	{
	public:
		CatcherManager();
		~CatcherManager();

		void Create(bool drums, bool six, unsigned int catchers = 5);
		void Destroy();

		void Update(float delta);

		void GetCatcherAmount(unsigned int *size);
		Catcher *operator[](int index);

		bool IsDrums();
		bool IsSix();
		unsigned int Size();

		void Hit(int catchers);

		int GetCatchersActive();
		int GetLanesActive();
		void SetActiveCatchers(int catchers);

		void SetCatchersActivationStatus(int catcher, bool activated);

	  private:
		Catcher			*m_catchers;
		unsigned int	m_size;

		bool			m_drums;
		bool			m_six;
	};
} // namespace Rhythmic

#endif
