#include "Application.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>

#if CRYSTAL_PLATFORM_WINDOWS
#include <windows.h>

fdsafdsaf
int WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow
    )
{
	Crystal::Application app = Crystal::Application();
	return 0;
}

#else

int main(int argc, char **argv)
{
	Crystal::Application app = Crystal::Application();
	return 0;
}

#endif