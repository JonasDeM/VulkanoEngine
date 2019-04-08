#pragma once
class Transform;

class TransformSystem
{
public:
	static void Awake(Transform data);
	//static void Start(Transform data);
	static void Update(Transform data);
	static void RecordVulkanDrawCommands(Transform data);
	//static void Stop(Transform data);
	static void Destroy(Transform data);
};