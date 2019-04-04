#pragma once
#include "Component.h"
#include "TransformFlags.h"
#include <glm\glm.hpp>
using namespace glm;

struct TransformData;

// Component that manages the position rotation and scale of objects
// Every Gameobject has a Transform
class Transform : public Component<TransformData>
{
public:
	// Hierarchy
	Transform GetParent();
	Transform GetRootParent(); // Maybe keep rootparent handle in TransformData?
	void AddChild(Transform child);
	void RemoveChild(Transform child);

	// 3D Position - Rotation - Scale
	void SetPosition(const vec3& pos);
	vec3 GetPosition();
	void SetRotation(const quat& rot);
	quat GetRotation();
	void SetEulerRotation(const vec3& rot);
	vec3 GetEulerRotation();
	void SetEulerRotationInDegrees(const vec3& rot);
	vec3 GetEulerRotationInDegrees();
	void SetScale(const vec3& scale);
	vec3 GetScale();

	void Translate(const vec3& delta);
	void Rotate(const quat& delta);
	void RotateEuler(const vec3& delta);
	void RotateEulerDegrees(const vec3& delta);
	void Scale(const vec3& delta);

	mat4 GetWorldMatrix();
private:
	void CalculateWorldMatrix();

private:
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};
};


struct TransformData {
	std::vector<Transform> m_Children;	// 32 bytes - 8 byte aligned
	Transform m_Parent;					// 8  bytes - 4 byte aligned
												  
	mat4 m_WorldMatrix;					// 64 bytes - 4 byte aligned
	vec3 m_Position;					// 12 bytes - 4 byte aligned
	vec3 m_WorldPosition;				// 12 bytes - 4 byte aligned
	vec3 m_Scale;						// 12 bytes - 4 byte aligned
	vec3 m_WorldScale;					// 12 bytes - 4 byte aligned
	quat m_Rotation;					// 16 bytes - 4 byte aligned
	quat m_WorldRotation;				// 16 bytes - 4 byte aligned
												  
	TransformFlags m_Flags;				// 1  byte  - 1 byte aligned
												  
private:										  
	char _padding[7];					// 7  bytes padding adds everything up to 192 bytes
};