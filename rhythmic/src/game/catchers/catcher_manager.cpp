#include "catcher_manager.hpp"

#include <assert.h>
#include <iostream>

namespace Rhythmic
{
	CatcherManager::CatcherManager() : 
		m_catchers(0),
		m_size(0)
	{}
	CatcherManager::~CatcherManager()
	{}

	void CatcherManager::Create(bool drums, unsigned int catchers)
	{
		assert(catchers == 5 || catchers == 4);
		if (m_size == 0)
		{
			m_drums = drums;
			m_catchers = new Catcher[5];
			m_size = catchers;
			

			/*if (drums)
			{
				assert(m_size == 5 || m_size == 4);
				if (m_size == 5)
				{
					m_catchers[0].type = CATCHER_TYPE_GREEN;
					m_catchers[1].type = CATCHER_TYPE_RED;
					m_catchers[2].type = CATCHER_TYPE_YELLOW;
					m_catchers[3].type = CATCHER_TYPE_BLUE;
					m_catchers[4].type = CATCHER_TYPE_ORANGE;
				}
			}*/

			m_catchers[0].type = drums ? CATCHER_TYPE_RED : CATCHER_TYPE_GREEN;
			m_catchers[1].type = drums ? CATCHER_TYPE_YELLOW : CATCHER_TYPE_RED;
			m_catchers[2].type = drums ? CATCHER_TYPE_BLUE : CATCHER_TYPE_YELLOW;
			m_catchers[3].type = drums ? (catchers == 4 ? CATCHER_TYPE_GREEN : CATCHER_TYPE_ORANGE) : CATCHER_TYPE_BLUE;
			m_catchers[4].type = drums ? CATCHER_TYPE_GREEN : CATCHER_TYPE_ORANGE;
			
			for (int i = 0; i < 5; i++)
			{
				m_catchers[i].hit_height = 0;
				m_catchers[i].active = false;
			}
		}
	}

	void CatcherManager::Update(float delta)
	{
		if (m_size <= 0)
			return;

		for (unsigned int i = 0; i < 5; i++)
		{
			if (m_catchers[i].hit_height > 0)
				m_catchers[i].hit_height -= delta * 0.66f;
		}
	}

	void CatcherManager::Destroy()
	{
		if (m_size > 0)
		{
			delete[] m_catchers;
			m_catchers = 0;
			m_size = 0;
		}
	}

	void CatcherManager::GetCatcherAmount(unsigned int *size)
	{
		*size = m_size;
	}

	void CatcherManager::Hit(int catchers)
	{
		if (m_size != 0)
		{
			if (catchers == 0)
			{
				for (unsigned int i = 0; i < 5; i++)
				{
					m_catchers[i].hit_height = g_catcher_hitHeight;
				}
				return;
			}

			for (unsigned int i = 0; i < 5; i++)
			{
				if (catchers & (1 << i))
					m_catchers[i].hit_height = g_catcher_hitHeight;
			}
		}
	}

	int CatcherManager::GetCatchersActive()
	{
		int ret = 0;
		for (unsigned int i = 0; i < 5; i++)
		{
			if (m_catchers[i].active)
				ret |= (1 << m_catchers[i].type);
		}
		return ret;
	}

	void CatcherManager::SetActiveCatchers(int active)
	{
		for (int i = 0; i < 5; i++)
			m_catchers[i].active = false;
		
		if (active & CATCHER_GREEN)
			m_catchers[0].active = true;
		if (active & CATCHER_RED)
			m_catchers[1].active = true;
		if (active & CATCHER_YELLOW)
			m_catchers[2].active = true;
		if (active & CATCHER_BLUE)
			m_catchers[3].active = true;
		if (active & CATCHER_ORANGE)
			m_catchers[4].active = true;
	}

	void CatcherManager::SetCatchersActivationStatus(int catcher, bool activated)
	{
		assert(catcher >= 0);
		assert(catcher < m_size);

		m_catchers[catcher].active = activated;
	}

	Catcher *CatcherManager::operator[](int index)
	{
		assert(index < 5);
		return &m_catchers[index];
	}

	bool CatcherManager::IsDrums()
	{
		return m_drums;
	}
	unsigned int CatcherManager::Size()
	{
		return m_size;
	}
} // namespace Rhythmic

