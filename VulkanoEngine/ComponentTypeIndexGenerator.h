#pragma once
class ComponentTypeIndexGenerator
{
public:
	static const uint16_t GenerateTypeIndex() noexcept
	{
		return m_Counter++;
	}
private:
	static uint16_t m_Counter;
};

