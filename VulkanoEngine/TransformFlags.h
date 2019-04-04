#pragma once
#include <cstdint>

class TransformFlags {

public:
	// Register Changes
	void PositionChanged();
	void RotationChanged();
	void ScaleChanged();

	// QueryChanges
	bool HasTransformChanged();
	bool HasPositionChanged();
	bool HasRotationChanged();
	bool HasScaleChanged();

private:
	enum TransformFlag : char
	{
		NONE = 0 << 0,
		POSITION_CHANGED = 1 << 0,
		ROTATION_CHANGED = 1 << 1,
		SCALE_CHANGED = 1 << 2,
	};
	TransformFlag Flags = TransformFlag::NONE;
};