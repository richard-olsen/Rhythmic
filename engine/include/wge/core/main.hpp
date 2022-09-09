#ifndef WGE_CORE_MAIN_HPP_
#define WGE_CORE_MAIN_HPP_

int gameEntry(int argc, char *argv[]);

void _initEngine();

#undef main
#ifdef WIN32
#ifdef WGE_DEBUG
int main(int argc, char *argv[])
{
	_initEngine();
	return gameEntry(argc, argv);
}
#else
#define NOMINMAX
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, INT nShowCmd)
//int main(int argc, char *argv[])
{
	_initEngine();
	return gameEntry(0, 0);
}
#endif
#else
int main(int argc, char *argv[])
{
	_initEngine();
	return gameEntry(argc, argv);
}
#endif



#endif

