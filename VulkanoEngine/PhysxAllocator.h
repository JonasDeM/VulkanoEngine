#pragma once
#include <malloc.h>
#include "foundation/PxAllocatorCallback.h"
using namespace physx;

class PhysxAllocator: public PxAllocatorCallback
{
public:
	PhysxAllocator(void){};
	~PhysxAllocator(void){};

	virtual void* allocate(size_t size, const char*, const char*, int)
	{
		return _aligned_malloc(size, 16);
	}

	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

