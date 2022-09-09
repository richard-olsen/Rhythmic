#include "binding_parser.hpp"

#include <wge/io/filesystem.hpp>

#include <fstream>

#include "misc.hpp"

#include <wge/core/logging.hpp>

#include <cstring>

#include <stdint.h>

#define WBC_MAGIC 1598243415
#define WBC_VERSION 0x01

#define WBC_PADDING 15

struct WBC_FileHeader
{
	uint32_t beginning;
	uint32_t fileSize;
	uint8_t version;
	uint16_t binding_count;
	uint8_t controller_type;
	WGE::Util::GUID controllerGuid;
	uint8_t axisCount;
	uint8_t padding[WBC_PADDING];
};

namespace Rhythmic
{
	void Read_WBC(const std::string &file, ContainerBinding *binding)
	{
		std::ifstream input(file, std::ios::binary);

		if (!input.is_open())
			return;

		binding->binding.clear();

		char guid[37];
		memset(guid, 0, 37);

		WBC_FileHeader wbc = { 0 };

		input.read((char *)&wbc.beginning, 4);
		input.read((char *)&wbc.fileSize, 4);
		input.read((char *)&wbc.version, 1);
		input.read((char *)&wbc.binding_count, 2);
		input.read((char *)&wbc.controller_type, 1);
		input.read(guid, 36);
		wbc.controllerGuid = WGE::Util::GUID(guid);
		input.read((char *)&wbc.axisCount, 1);
		for (size_t i = 0; i < WBC_PADDING; i++)
		{
			input.read((char *)&wbc.padding[i], 1);
		}
		

		for (uint16_t i = 0; i < wbc.binding_count; i++)
		{
			uint8_t		type;
			uint16_t	gameButton;
			uint32_t	rawButton;

			input.read((char *)&type, 1);
			input.read((char *)&gameButton, 2);
			input.read((char *)&rawButton, 4);

			InputRawButton raw;
			raw.axis			= type == 1 ? rawButton : -1;
			raw.keybutton	= type == 2 ? rawButton : -1;
			raw.midi			= type == 3 ? rawButton : -1;
			raw.button		= type == 0 ? rawButton : -1;

			InputBindingsAddButton(&binding->binding, raw, (InputGameButton)gameButton);
		}

		binding->calibrations.resize(wbc.axisCount);

		for (uint8_t i = 0; i < wbc.axisCount; i++)
		{
			InputAxisCalibration &calibration = binding->calibrations[i];

			input.read((char *)&calibration.min, 4);
			input.read((char *)&calibration.max, 4);
		}
	}
	void Save_WBC(InputDevice *device)
	{
		if (!device)
			return;

		ContainerBinding binding;
		binding.binding = device->binding;
		binding.calibrations = device->axisCalibrations;

		Save_WBC(device->guid, &binding);
	}

	void Save_WBC(const WGE::Util::GUID &guid, const ContainerBinding *binding)
	{
		if (!binding)
			return;

		std::string sGuid = guid.ToString();
		std::ofstream output(WGE::FileSystem::GetFileInDirectory("/configs/" + sGuid + ".wbc"), std::ios::binary);

		if (!output.is_open())
			return;

		output.clear();

		WBC_FileHeader headerData;
		headerData.beginning = WBC_MAGIC;
		headerData.fileSize = 0;
		headerData.version = WBC_VERSION;
		headerData.binding_count = 0;
		headerData.controller_type = 0;
		headerData.axisCount = binding->calibrations.size();

		unsigned char headerOut[64];

		Util::Serialize_LE(headerData.beginning, &headerOut[0]);
		headerOut[8] = headerData.version;
		headerOut[11] = headerData.controller_type;
		memcpy(&headerOut[12], sGuid.c_str(), 36);

		Util::Serialize_LE(headerData.axisCount, &headerOut[48]);

		output.write((char *)headerOut, 64);

		for (auto it = binding->binding.begin(); it != binding->binding.end(); it++)
		{
			// 0 = button
			// 1 = axis
			// 2 = key
			// 3 = midi
			char type = it->first.axis >= 0 ? 1 : (it->first.keybutton >= 0 ? 2 : (it->first.midi >= 0 ? 3 : 0));
			int button = -1;
			switch (type)
			{
			case 1:		button = it->first.axis; break;
			case 2:		button = it->first.keybutton; break;
			case 3:		button = it->first.midi; break;
			default:	button = it->first.button; break;
			}

			// Save all game buttons
			for (auto it_b = it->second.begin(); it_b != it->second.end(); it_b++) // Heh, good naming...
			{
				union
				{
					unsigned char value[4];
					int value_i;
				};

				/*
				1 byte bind type (button, midi, keyboard button, axis etc.)
				2 byte game button
				4 byte raw button (button id (x axis, b button, a key, 12 midi))
				*/

				// Button Type
				output.put(type);
				value_i = *it_b;
				// Game Button
				output.put(value[0]);
				output.put(value[1]);

				value_i = button;
				// Raw Button
				output.put(value[0]);
				output.put(value[1]);
				output.put(value[2]);
				output.put(value[3]);

				headerData.binding_count++;
			}
		}

		for (uint8_t i = 0; i < headerData.axisCount; i++)
		{
			const InputAxisCalibration &calibration = binding->calibrations[i];
			unsigned char outCalibration[8];
			Util::Serialize_LE(calibration.min, &outCalibration[0]);
			Util::Serialize_LE(calibration.max, &outCalibration[4]);
			output.write((char *)outCalibration, 8);
		}

		Util::Serialize_LE(headerData.binding_count, &headerOut[9]);

		headerData.fileSize = (unsigned int) output.tellp();
		Util::Serialize_LE(headerData.fileSize, &headerOut[4]);

		output.seekp(9, std::ios::beg);
		output.write((char*)&headerOut[9], 2);
		output.seekp(4, std::ios::beg);
		output.write((char *)&headerOut[4], 4);
	}
}