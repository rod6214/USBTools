#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <iostream>
#include <combaseapi.h>
#include <SetupAPI.h>
#include <iostream>
#include <regex>
#include <memory>
#include <thread>
//#include <windows.data.json.h>
#include <string>
#include <vector>

#include <Windows.h>
#include <winusb.h>
#include <future>
//#include <json/json.h>
#include <stdlib.h>
#include <string>
#include <direct.h>
#include <fstream>
#include <sstream>



#ifndef POTYFAST_EXPORTS
#define DLLIMPORT __declspec(dllimport)
#else
#define DLLIMPORT __declspec(dllexport)
#endif
