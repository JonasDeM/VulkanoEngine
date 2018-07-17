#pragma once

// VULKAN
//*******
#include <vulkan/vulkan.h>

// GLM
//****
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

// PHYSX
//******
#include <PxPhysicsAPI.h>
using namespace physx;
#if (defined(DEBUG) || defined(_DEBUG)) && defined(_M_IX86)
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#endif
#if (defined(RELEASE) || defined(NDEBUG)) && defined(_M_IX86)
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#endif
#if (defined(DEBUG) || defined(_DEBUG)) && defined(_M_X64)
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#endif
#if (defined(RELEASE) || defined(NDEBUG)) && defined(_M_X64)
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDK.lib")
#endif
//PhysX:you must suppress its definition of the macros min and max, as these are common C++ method names in the SDK
#define NOMINMAX

// C RunTime Header Files
//***********************
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <crtdbg.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <io.h>
#include <unordered_map>
#include <algorithm>	
#include <map>	
#include <vector>
#include <memory>
using namespace std;