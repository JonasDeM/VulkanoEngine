#pragma once
#include "foundation/PxErrorCallback.h"
using namespace physx;

class PhysxErrorCallback: public PxErrorCallback
{
public:
	PhysxErrorCallback(void);
	~PhysxErrorCallback(void);

	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line);
};