#pragma once
struct VertexPosCol;
class GameContext;
class VulkanContext;
class VkDebugPipeline_Ext;
class VkPhysicalDevice_Ext;
#include "HandleUtilities.h"
#include <vulkan\vulkan.hpp>
class GameScene;

class VulkanDebugRenderer
{
public:
	VulkanDebugRenderer(VulkanContext* vulkanContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize) ;
	~VulkanDebugRenderer();

	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, unsigned int vertexCount, const int frameBufferIndex);

	void UpdateUniformVariables(const VulkanContext* pVkContext, const GameScene* pCurrentScene);
	void UpdateVertexData(const VulkanContext* pVkContext, const vector<VertexPosCol>& lineList, unsigned int fixedBufferSize);
	void CreateVertexBuffer(VulkanContext* pVkContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize);
	void CreateUniformBuffer(const VulkanContext* pVkContext);

private:
	std::vector<unique_ptr_del<VkBuffer>> m_VertexBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_VertexBuffersMemory;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

};

