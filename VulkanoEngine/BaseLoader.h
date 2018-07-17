#pragma once
#include <typeinfo>

class BaseLoader
{
public:
	BaseLoader() {};
	virtual const type_info& GetType() const = 0;

	BaseLoader(const BaseLoader &obj) = delete;
	BaseLoader& operator=(const BaseLoader& obj) = delete;
private:
};

class VkBaseLoader
{
public:
	VkBaseLoader() {};
	virtual const type_info& GetType() const = 0;

	VkBaseLoader(const VkBaseLoader &obj) = delete;
	VkBaseLoader& operator=(const VkBaseLoader& obj) = delete;
private:
};