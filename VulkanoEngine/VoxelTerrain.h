#pragma once
#include "GameObject.h"
#include <array>

class MeshData;
class VkBasicSampler_Ext;
class VkTextureImage_Ext;
class VkImageView_Ext;
struct VertexPosColNorm;


class VoxelTerrain : public GameObject
{
public:
	VoxelTerrain(float sizeVoxel);
	virtual ~VoxelTerrain(void) = default;
	VoxelTerrain(const VoxelTerrain& t) = delete;
	VoxelTerrain& operator=(const VoxelTerrain& t) = delete;

	void Initialize(VulkanContext* pVkContext) override;
	void Update(VulkanContext* pVkContext) override;
	
	void UpdateVertexBufferSize(VulkanContext* pVkContext);

protected:
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;
private:
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void NormalsToVertices();
	vec3 IndexToXYZ(int index);

	unique_ptr_del<VkBuffer> m_pVertexBuffer;
	int m_BufferSize = 0;
	unique_ptr_del<VkDeviceMemory> m_pVertexBufferMem;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	static const int AMOUNT_1_AXIS = 5;
	float m_SizeVoxel = 1.0f;
	std::array<vec3, AMOUNT_1_AXIS*AMOUNT_1_AXIS*AMOUNT_1_AXIS> m_Normals;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

	std::vector<VertexPosColNorm> m_Vertices;


};