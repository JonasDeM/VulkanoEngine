#pragma once

inline PxVec3 ToPxVec3(vec3 vec)
{
	return PxVec3(vec.x, vec.y, vec.z);
}

inline PxVec2 ToPxVec2(vec2 vec)
{
	return PxVec2(vec.x, vec.y);
}

inline PxVec4 ToPxVec4(vec4 vec)
{
	return PxVec4(vec.x, vec.y, vec.z, vec.w);
}

inline PxQuat ToPxQuat(vec4 vec)
{
	return PxQuat(vec.x, vec.y, vec.z, vec.w);
}
inline vec3 Tovec3(PxVec3 vec)
{
	return vec3(vec.x, vec.y, vec.z);
}