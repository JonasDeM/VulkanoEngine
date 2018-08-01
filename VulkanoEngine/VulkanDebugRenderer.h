#pragma once

#include "HandleUtilities.h"
#include "VkDebugPipeline_Ext.h"

class VulkanContext;
class VkPhysicalDevice_Ext;
class GameScene;

class VulkanDebugRenderer
{
public:
	typedef struct VkDebugPipeline_Ext::VertexType VertexType; // convienience

	VulkanDebugRenderer(VulkanContext* vulkanContext, const vector<VertexType>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize) ;
	~VulkanDebugRenderer() = default;

	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, unsigned int vertexCount, const int frameBufferIndex);

	void UpdateUniformVariables(const VulkanContext* pVkContext, const GameScene* pCurrentScene);
	void UpdateVertexData(const VulkanContext* pVkContext, const vector<VertexType>& lineList, unsigned int fixedBufferSize);
	void CreateVertexBuffer(VulkanContext* pVkContext, const vector<VertexType>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize);
	void CreateUniformBuffer(const VulkanContext* pVkContext);

private:

	std::vector<unique_ptr_del<VkBuffer>> m_VertexBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_VertexBuffersMemory;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

};

