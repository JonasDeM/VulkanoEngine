#pragma once
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "HandleUtilities.h"
#include "VkImageView_Ext.h"


class CubePosColorNorm : public GameObject
{
public:
	CubePosColorNorm(float width, float height, float depth);
	CubePosColorNorm(float width, float height, float depth, vec4 color);
	~CubePosColorNorm(void);

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateVertexBuffer(VulkanContext* pVkContext);
	void CreateIndexBuffer(VulkanContext* pVkContext);
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) override;

	float m_Width, m_Height, m_Depth;
	static const int NUMVERTICES = 24;
	static const int NUMINDICES = 36;

	unique_ptr_del<VkBuffer> m_VertexBuffer;
	unique_ptr_del<VkDeviceMemory> m_VertexBufferMemory;
	unique_ptr_del<VkBuffer> m_IndexBuffer;
	unique_ptr_del<VkDeviceMemory> m_IndexBufferMemory;
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

	bool m_UseCustomColor = false;
	vec4 m_CustomColor;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	CubePosColorNorm(const CubePosColorNorm& t);
	CubePosColorNorm& operator=(const CubePosColorNorm& t);
};
