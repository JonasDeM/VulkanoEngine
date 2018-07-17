#pragma once
//this way you can derive of underiveable types like int
//usefull for adding functionality to handles
//it also enables the automatic cleanup
#include <vulkan/vulkan.h>

template<typename T>
class DeriveableHandle
{
public:
	DeriveableHandle() = default;
	DeriveableHandle(T baseObject) : m_BaseObject(baseObject){}

	operator T() const
	{
		return m_BaseObject;
	}	
protected:
	T* BasePtr()
	{
		return &m_BaseObject;
	}

private:
	T m_BaseObject = VK_NULL_HANDLE;
};
