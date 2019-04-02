#pragma once
#include "stdafx.h"
#include "MainGame.h"
#include <limits>

int main(int arc, char** argv)
{
	// Enable run-time memory leak check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(412);
	#endif
	auto gamePtr = std::make_unique<MainGame>();
	gamePtr->RunGame();
}
