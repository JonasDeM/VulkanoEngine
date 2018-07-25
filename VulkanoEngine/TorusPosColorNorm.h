#pragma once

#include "GameObject.h"
#include "Colors.h"
class VkBasicSampler_Ext;
class VkTextureImage_Ext;
class VkImageView_Ext;
class VkPosColNormPipeline_Ext;


class TorusPosColorNorm : public GameObject
{
public:
	TorusPosColorNorm(float majorRadius, uint16_t majorRadiusSteps, float minorRadius, uint16_t minorRadiusSteps, vec4 color = glm::Green);
	~TorusPosColorNorm(void);

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateVertexBuffer(VulkanContext* pVkContext);
	void CreateIndexBuffer(VulkanContext* pVkContext);
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;

	float m_MajorRadius, m_MinorRadius;
	uint16_t m_MajorRadiusSteps = 3, m_MinorRadiusSteps = 2, m_NumVerts = 0;
	uint32_t m_NumIndices = 0;
	vec4 m_Color;


	unique_ptr_del<VkBuffer> m_VertexBuffer = nullptr;
	unique_ptr_del<VkDeviceMemory> m_VertexBufferMemory = nullptr;
	unique_ptr_del<VkBuffer> m_IndexBuffer = nullptr;
	unique_ptr_del<VkDeviceMemory> m_IndexBufferMemory = nullptr;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	TorusPosColorNorm(const TorusPosColorNorm& t);
	TorusPosColorNorm& operator=(const TorusPosColorNorm& t);
};
