#ifndef FRETBOARD_CONTAINER_H_
#define FRETBOARD_CONTAINER_H_

#include <string>
#include <vector>

#include "fretboard.hpp"

namespace Rhythmic
{
	namespace FretboardContainer
	{
		void Initialize();
		void Dispose();

		void Recache();

		/*
		Returns the fretboard object itself with the given handle

		Will return NULL if the handle isn't valid
		*/
		Fretboard *GetFretboard(FretboardHandle index);
		/*
		Returns a handle to the fretboard with the given name
		*/
		FretboardHandle FindFretboard(const std::string &name);

		/*
		Returns the next valid fretboard handle. Useful for switching fretboards
		*/
		FretboardHandle GetNextValidHandle(FretboardHandle handle);
		/*
		Returns the previous valid fretboard handle. Useful for switching fretboards
		*/
		FretboardHandle GetPrevValidHandle(FretboardHandle handle);
	}
}

#endif
