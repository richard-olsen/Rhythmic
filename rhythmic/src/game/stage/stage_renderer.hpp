#ifndef STAGE_RENDERER_HPP_
#define STAGE_RENDERER_HPP_

namespace Rhythmic
{
	class StageRenderer
	{
	public:
		static StageRenderer *GetInstance();

		void Render(double interpolation = 0);
	private:
		StageRenderer();
	};
}

#endif

