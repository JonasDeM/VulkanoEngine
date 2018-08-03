#pragma once
#include <typeinfo>

class BaseLoader
{
public:
	BaseLoader() = default;
	virtual ~BaseLoader() = default;
	BaseLoader(const BaseLoader &obj) = delete;
	BaseLoader& operator=(const BaseLoader& obj) = delete;
private:
};
