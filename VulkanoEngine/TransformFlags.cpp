#include "stdafx.h"
#include "TransformFlags.h"

void TransformFlags::PositionChanged()
{
	Flags = static_cast<TransformFlag>(Flags & TransformFlag::POSITION_CHANGED);
}

void TransformFlags::RotationChanged()
{
	Flags = static_cast<TransformFlag>(Flags & TransformFlag::ROTATION_CHANGED);
}

void TransformFlags::ScaleChanged()
{
	Flags = static_cast<TransformFlag>(Flags & TransformFlag::SCALE_CHANGED);
}

bool TransformFlags::HasTransformChanged()
{
	return Flags & (TransformFlag::POSITION_CHANGED | TransformFlag::ROTATION_CHANGED | TransformFlag::SCALE_CHANGED);
}

bool TransformFlags::HasPositionChanged()
{
	return Flags & TransformFlag::POSITION_CHANGED;
}

bool TransformFlags::HasRotationChanged()
{
	return Flags & TransformFlag::ROTATION_CHANGED;
}

bool TransformFlags::HasScaleChanged()
{
	return Flags & TransformFlag::SCALE_CHANGED;
}