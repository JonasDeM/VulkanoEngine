#pragma once
#include "stdafx.h"
#include "PhysxErrorCallback.h"
#include "Debug.h"
using namespace std;

PhysxErrorCallback::PhysxErrorCallback(void){}
PhysxErrorCallback::~PhysxErrorCallback(void){}

void PhysxErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	wstringstream ss;
	ss<<L"PHYSX ERROR REPORT:"<<endl;
	ss<<"Error Code: "<<code<<endl;
	ss<<"Message: "<<message<<endl;
	ss<<"File: "<<file<<" (line: "<<line<<")";

	Debug::LogError(ss.str());
}