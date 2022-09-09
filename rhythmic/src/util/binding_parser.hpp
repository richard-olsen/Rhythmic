#ifndef RHYTHMIC_BINDING_PARSER_HPP_
#define RHYTHMIC_BINDING_PARSER_HPP_

#include <wge/util/guid.hpp>
#include "../input/input_structures.hpp"

namespace Rhythmic
{
	enum WBC_Status
	{
		WBC_READ_OK,

		WBC_READ_INVALID_HEADER,
	};

	void Read_WBC(const std::string &file, ContainerBinding *binding);
	void Save_WBC(InputDevice *device);
	void Save_WBC(const WGE::Util::GUID &guid, const ContainerBinding *binding);
}


#endif

