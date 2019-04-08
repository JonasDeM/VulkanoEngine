#pragma once
#include "ComponentData.h"
#include "Transform.h"
#include "TransformFlags.h"

struct TransformData : public ComponentData
{
	std::vector<Transform> m_Children;	// 32 bytes - 8 byte aligned					//
	Transform m_Parent;					// 8  bytes - 8 byte aligned					//
	TransformFlags m_Flags;				// 2  bytes - 2 byte aligned					//-> 64 bytes (together with base class members)
private:																				//
	char __padding[8 - sizeof(TransformFlag)]; // just some explicit padding			//

public:
	mat4 m_WorldMatrix;					// 64 bytes - 4 byte aligned					//-> 64 bytes

	vec3 m_LocalPosition;				// 12 bytes - 4 byte aligned					//
	quat m_LocalRotation;				// 16 bytes - 4 byte aligned					//
	vec3 m_LocalScale;					// 12 bytes - 4 byte aligned					//
	vec3 m_WorldPosition;				// 12 bytes - 4 byte aligned					//-> 68 bytes
	quat m_WorldRotation;				// 12 bytes - 4 byte aligned					//
	vec3 m_WorldScale;					// 12 bytes - 4 byte aligned					//
};
