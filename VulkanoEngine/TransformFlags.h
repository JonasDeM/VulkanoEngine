#pragma once
#include <cstdint>

using TransformFlagType = int16_t;

enum TransformFlag : TransformFlagType 
{
	NONE = 0 << 0,
	WORLD_MATRIX_OUTOFDATE = 1 << 0,
	WORLD_POSITION_OUTOFDATE = 1 << 1,
	WORLD_ROTATION_OUTOFDATE = 1 << 2,
	WORLD_SCALE_OUTOFDATE = 1 << 3,
	PHYSICS_POSITION_OUTOFDATE = 1 << 4,
	PHYSICS_ROTATION_OUTOFDATE = 1 << 5,
	PHYSICS_SCALE_OUTOFDATE = 1 << 6,
};
inline TransformFlag operator|(TransformFlag a, TransformFlag b)
{
	return static_cast<TransformFlag>(static_cast<TransformFlagType>(a) | static_cast<TransformFlagType>(b));
}
inline TransformFlag operator&(TransformFlag a, TransformFlag b)
{
	return static_cast<TransformFlag>(static_cast<TransformFlagType>(a) & static_cast<TransformFlagType>(b));
}
inline TransformFlag operator~(TransformFlag a)
{
	return static_cast<TransformFlag>(~static_cast<TransformFlagType>(a));
}

class TransformFlags {

public:
	bool HasFlags(TransformFlag flags);
	void SetFlags(TransformFlag flag);
	void ClearFlags(TransformFlag flag);

private:
	TransformFlag m_Flags = TransformFlag::NONE;
};