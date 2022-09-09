#ifndef MEASURE_H_
#define MEASURE_H_

namespace Rhythmic
{
	enum BeatlineType
	{
		BEATLINE_TYPE_FULL,
		BEATLINE_TYPE_HALF
	};

	struct Beatline
	{
		float time;
		BeatlineType type;
	};
}

#endif

