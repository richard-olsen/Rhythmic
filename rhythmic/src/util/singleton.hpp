#ifndef RHYTHMIC_UTIL_SINGLETON_HPP_
#define RHYTHMIC_UTIL_SINGLETON_HPP_

// Just incase I want to add more than just GetInstance for singletons

namespace Rhythmic
{
	template<typename T>
	class Singleton
	{
	public:
		static T *GetInstance()
		{
			static T instance;
			return &instance;
		}
	};
}

#endif

