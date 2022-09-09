#include "canvas_factory.hpp"

#include "types/canvas_main.hpp"
#include "types/canvas_song_select.hpp"
#include "types/canvas_players.hpp"
#include "types/canvas_song_prep.hpp"
#include "types/canvas_song_current.hpp"
#include "types/canvas_song_stats.hpp"
#include "types/canvas_section_select.hpp"
#include "types/canvas_extras.hpp"
#include "types/canvas_options.hpp"
#include "types/canvas_pause_settings.hpp"
#include "types/canvas_bindings.hpp"
#include "types/canvas_auto_binder.hpp"
#include "types/canvas_binding_option.hpp"
#include "types/canvas_engine_creator.hpp"
#include "types/canvas_engine_creator_editor.hpp"
#include "types/canvas_secret_unlock.hpp"
#include "types/canvas_calibration.hpp"

namespace Rhythmic
{
	Canvas *CanvasFactory::CreateCanvas(CanvasFactoryType type, void *canvasData)
	{
		Canvas *canvas = new Canvas();
		switch (type)
		{
		case CANVAS_FACTORY_MAIN:
			FactoryCanvas::CanvasCreateMain(canvas, canvasData);
			break;
		case CANVAS_FACTORY_SONG_SELECTION:
			FactoryCanvas::CanvasCreateSongSelect(canvas, canvasData);
			break;
		case CANVAS_FACTORY_PLAYERS_SETTINGS:
			FactoryCanvas::CanvasCreatePlayers(canvas, canvasData);
			break;
		case CANVAS_FACTORY_SONG_PREP:
			FactoryCanvas::CanvasCreateSongPrep(canvas, canvasData);
			break;
		case CANVAS_FACTORY_IN_GAME_HUD:
			FactoryCanvas::CanvasCreateSongCurrent(canvas, canvasData);
			break;
		case CANVAS_FACTORY_SONG_STATS:
			FactoryCanvas::CanvasCreateSongStats(canvas, canvasData);
			break;
		case CANVAS_FACTORY_SECTION_SELECTION:
			FactoryCanvas::CanvasCreateSectionSelection(canvas, canvasData);
			break;
		case CANVAS_FACTORY_EXTRAS:
			FactoryCanvas::CanvasCreateExtras(canvas, canvasData);
			break;
		case CANVAS_FACTORY_OPTIONS:
			FactoryCanvas::CanvasCreateOptions(canvas, canvasData);
			break;
		case CANVAS_FACTORY_PAUSE_OPTIONS:
			FactoryCanvas::CanvasCreatePauseOptions(canvas, canvasData);
			break;
		case CANVAS_FACTORY_BINDINGS:
			FactoryCanvas::CanvasCreateBindings(canvas, canvasData);
			break;
		case CANVAS_FACTORY_AUTO_BINDER:
			FactoryCanvas::CreateCanvasAutoBinder(canvas, canvasData);
			break;
		case CANVAS_FACTORY_BINDING_OPTIONS:
			FactoryCanvas::CanvasCreateBindingOptions(canvas, canvasData);
			break;
		case CANVAS_FACTORY_ENGINE_CREATOR:
			FactoryCanvas::CanvasCreateEngineCreator(canvas, canvasData);
			break;
		case CANVAS_FACTORY_ENGINE_CREATOR_EDITOR:
			FactoryCanvas::CanvasCreateEngineCreatorEditor(canvas, canvasData);
			break;
		case CANVAS_FACTORY_SECRET_UNLOCK:
			FactoryCanvas::CanvasCreateSecretUnlock(canvas, canvasData);
			break;
		case CANVAS_FACTORY_CALIBRATION:
			FactoryCanvas::CanvasCreateCalibration(canvas, canvasData);
			break;
		default:
			delete canvas;
			return 0;
		}
		canvas->SetType(type);
		return canvas;
	}
}
