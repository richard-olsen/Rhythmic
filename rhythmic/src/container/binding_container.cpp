#include "binding_container.hpp"

#include <filesystem>

#include "../util/misc.hpp"

#include <map>
#include <string>

#include "../util/binding_parser.hpp"

#include "../input/input.hpp"


namespace Rhythmic
{
	namespace BindingContainer
	{
		std::map<WGE::Util::GUID, ContainerBinding> g_bindings;

		void Initialize()
		{
			Recache();
		}
		void Dispose()
		{

		}

		void Recache()
		{
			g_bindings.clear();
			
			std::string path = Util::GetExecutablePath() + "/configs";

			if (!std::filesystem::is_directory(path))
				std::filesystem::create_directory(path);

			for (std::filesystem::directory_iterator it = std::filesystem::directory_iterator(path); it != std::filesystem::directory_iterator(); it++)
			{
				std::filesystem::path path = it->path();
				if (Util::compareStringCaseInsensitive(path.extension().string(), ".wbc") == 0 && path.filename().string().size() == 40)
				{
					ContainerBinding bindings;
					Read_WBC(path.string(), &bindings);
					
					std::string sGuid = path.filename().string();
					sGuid = sGuid.substr(0, sGuid.size() - 4); // Remove the extension to get just the GUID

					WGE::Util::GUID guid = WGE::Util::GUID(sGuid);

					// Update as they are found
					InputSetAllBinding(guid, &bindings);

					g_bindings.insert(std::make_pair(guid, bindings));
				}
			}
		}


		const ContainerBinding *GetBinding(const WGE::Util::GUID &guid)
		{
			auto it = g_bindings.find(guid);
			
			if (it != g_bindings.end())
				return &it->second;
			
			return 0;
		}

		void InsertBinding(const WGE::Util::GUID &guid, const ContainerBinding &binding)
		{
			auto it = g_bindings.find(guid);

			if (it != g_bindings.end())
			{
				it->second = binding;
			}
			else
			{
				g_bindings.insert(std::make_pair(guid, binding));
			}

			Save_WBC(guid, &binding);
		}
	}
}

