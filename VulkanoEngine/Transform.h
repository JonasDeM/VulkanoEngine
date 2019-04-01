#pragma once
#include "Component.h"

struct TransformData;

class Transform : public Component<TransformData>
{
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};
};

struct TransformData {
	Transform Parent;
	std::vector<Transform> Children;
	glm::vec3 Position;
};

struct TransformData2;
class Transform2 : public Component<TransformData2>
{
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};
};

struct TransformData2 {
	Transform Parent;
	std::vector<Transform> Children;
	glm::vec3 Position;
};

int main(int arc, char** argv)
{
	GameObject1 GameObject = GameObject1::CreateNew();
	Transform t = GameObject.AddComponent<Transform>();
	Transform2 t2 = GameObject.AddComponent<Transform2>();
}