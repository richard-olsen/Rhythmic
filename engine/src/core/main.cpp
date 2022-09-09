#include <wge/core/logging.hpp>
#include <wge/io/filesystem.hpp>

void _initEngine()
{
	using namespace WGE;

	FileSystem::InitializeFileSystem();
	LoggerInit();
}

