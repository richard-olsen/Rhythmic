#ifndef CATCHER_H_
#define CATCHER_H_

namespace Rhythmic
{
	enum CatcherType
	{
		CATCHER_TYPE_GREEN,
		CATCHER_TYPE_RED,
		CATCHER_TYPE_YELLOW,
		CATCHER_TYPE_BLUE,
		CATCHER_TYPE_ORANGE,
	};

	const float g_catcher_hitHeight = 0.075f;

	struct Catcher
	{
		CatcherType type;
		bool active;

		// GH Style Values
		float hit_height;
	};
} // namespace Rhythmic

#endif
