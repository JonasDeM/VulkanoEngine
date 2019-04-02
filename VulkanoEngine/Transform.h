#pragma once
#include "Component.h"
#include <glm\glm.hpp>

struct TransformData;

class Transform : public Component<TransformData>
{
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};

public:
	void SetPosition(const glm::vec3& pos);
	void GetPosition(glm::vec3& pos);
};

struct TransformData {
	Transform Parent;
	std::vector<Transform> Children;
	glm::vec3 Position;
};
