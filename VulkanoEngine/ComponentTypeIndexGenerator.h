#pragma once
#include <limits>
using ComponentTypeIndex = uint16_t;

class ComponentTypeIndexGenerator
{
public:
	static const ComponentTypeIndex GenerateTypeIndex() noexcept
	{
		assert(m_Counter < std::numeric_limits<ComponentTypeIndex>::max());
		return m_Counter++;
	}
private:
	static ComponentTypeIndex m_Counter;
};

