#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include <memory>

template<typename T>
using unique_ptr_del = std::unique_ptr<T, std::function<void(T*)>>;

template<typename T>
unique_ptr_del<T> CreateExtendedHandle(T* created_with_new)
{
	return unique_ptr_del<T>(created_with_new, [](T* obj)
	{
		obj->Destroy();
		delete obj;
	});
}

template<typename T>
unique_ptr_del<T> CreateExtendedHandle(T* created_with_new, VkDevice device)
{
	return unique_ptr_del<T>(created_with_new, [device](T* obj)
	{
		obj->Destroy(device);
		delete obj;
	});
}

template<typename T>
unique_ptr_del<T> CreateExtendedHandle(T* created_with_new, VkInstance instance)
{
	return unique_ptr_del<T>(created_with_new, [instance](T* obj)
	{
		obj->Destroy(instance);
		delete obj;
	});
}

template<typename T>
std::shared_ptr<T> CreateSharedExtendedHandle(T* created_with_new)
{
	return std::shared_ptr<T>(created_with_new, [](T* obj)
	{
		obj->Destroy();
		delete obj;
	});
}

template<typename T>
std::shared_ptr<T> CreateSharedExtendedHandle(T* created_with_new, VkDevice device)
{
	return std::shared_ptr<T>(created_with_new, [device](T* obj)
	{
		obj->Destroy(device);
		delete obj;
	}); 
}

template<typename T>
std::shared_ptr<T> CreateSharedExtendedHandle(T* created_with_new, VkInstance instance)
{
	return std::shared_ptr<T>(created_with_new, [instance](T* obj)
	{
		obj->Destroy(instance);
		delete obj;
	});
}

template<typename T>
unique_ptr_del<T> CreateHandle(std::function<void(T, const VkAllocationCallbacks* allocPtr)> destroyer)
{
	return unique_ptr_del<T>(new T(), [destroyer](T* obj)
	{
		destroyer(*obj, nullptr);
		delete obj;
	});
}

template<typename T>
unique_ptr_del<T> CreateHandle(std::function<void(VkDevice, T, const VkAllocationCallbacks*)> destroyer, VkDevice device)
{
	return unique_ptr_del<T>(new T(), [destroyer, device](T* obj)
	{
		destroyer(device, *obj, nullptr);
		delete obj;
	});
}

template<typename T>
unique_ptr_del<T> CreateHandle(std::function<void(VkInstance, T, const VkAllocationCallbacks*)> destroyer, VkInstance instance)
{
	return unique_ptr_del<T>(new T(), [destroyer, instance](T* obj)
	{
		destroyer(instance, *obj, nullptr);
		delete obj;
	});
}

template<typename T>
std::shared_ptr<T> CreateSharedHandle(std::function<void(T, const VkAllocationCallbacks* allocPtr)> destroyer)
{
	return std::shared_ptr<T>(new T(), [destroyer](T* obj)
	{
		destroyer(*obj, nullptr);
		delete obj;
	});
}

template<typename T>
std::shared_ptr<T> CreateSharedHandle(std::function<void(VkDevice, T, const VkAllocationCallbacks*)> destroyer, VkDevice device)
{
	return std::shared_ptr<T>(new T(), [destroyer, device](T* obj)
	{
		destroyer(device, *obj, nullptr);
		delete obj;
	}); 
}

template<typename T>
std::shared_ptr<T> CreateSharedHandle(std::function<void(VkInstance, T, const VkAllocationCallbacks*)> destroyer, VkInstance instance)
{
	return std::shared_ptr<T>(new T(), [destroyer, instance](T* obj)
	{
		destroyer(instance, *obj, nullptr);
		delete obj;
	});
}

