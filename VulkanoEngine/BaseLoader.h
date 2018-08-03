#pragma once
#include <typeinfo>

class BaseLoader
{
public:
	BaseLoader() = default;
	virtual ~BaseLoader() = default;
	//virtual const type_info& GetType() const = 0;

	BaseLoader(const BaseLoader &obj) = delete;
	BaseLoader& operator=(const BaseLoader& obj) = delete;
private:
};
