#ifndef THEME_STRUCTURES_H_
#define THEME_STRUCTURES_H_

namespace Rhythmic
{
	typedef struct index2 { unsigned int index[2]; } index2;
	// Theme [Structure Name] [Allows] [Style]
	
	// "default_color": [0.0,1.0,0.0],
	// "flip": false,
	// "overlay": true,
	// "base": [0,1],
	// "base_cover": [0,2],
	// "inlet_inactive": [0,3],
	// "inlet_active": [0,4],
	// "inlet_active_overlay": [0,8],
	// "top": [0,5],
	// "top_overlay_inactive": [0,6],
	// "top_overlay_active": [0,7]

	struct ThemeCatchersFret
	{
		float default_color[3];
		bool flip;
		bool overlay;
		index2 base;
		index2 base_cover;
		index2 inlet_inactive;
		index2 inlet_active;
		index2 inlet_overlay;
		index2 top;
		index2 top_overlay_inactive;
		index2 top_overlay_active;
	};

	struct ThemeCatchers
	{
		ThemeCatchersFret frets[5];
		index2 extender_index;
		float extender_height;
		float extender_drop_speed;
	};
	
}

#endif

