#ifndef BINDING_CONTAINER_H_
#define BINDING_CONTAINER_H_

#include "../input/input_structures.hpp"

#include <wge/util/guid.hpp>

namespace Rhythmic
{
	/*
	This container will NOT contain any player specific bindings. This
	container is only to contain default bindings contained within the
	controller dictionary
	*/
	namespace BindingContainer
	{
		void Initialize();
		void Dispose();

		void Recache();

		/*
		Returns the binding pointer if one exists for the given GUID. Will return
		NULL if a set of bindings don't exist for the GUID
		*/
		const ContainerBinding *GetBinding(const WGE::Util::GUID &guid);

		/*
		Will add the binding into the map, or will update the bindings if one exists.
		This will always save a copy in the /configs/ of the game directory
		*/
		void InsertBinding(const WGE::Util::GUID &guid, const ContainerBinding &binding);
	}
}

#endif