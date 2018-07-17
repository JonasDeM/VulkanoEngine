#pragma once
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "VkPosColNormPipeline_Ext.h"
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
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer) override;

	float m_Width, m_Height, m_Depth;
	static const int NUMVERTICES = 24;
	static const int NUMINDICES = 36;

	unique_ptr_del<VkBuffer> m_VertexBuffer;
	unique_ptr_del<VkDeviceMemory> m_VertexBufferMemory;
	unique_ptr_del<VkBuffer> m_IndexBuffer;
	unique_ptr_del<VkDeviceMemory> m_IndexBufferMemory;
	unique_ptr_del<VkBuffer> m_UniformBuffer;
	unique_ptr_del<VkDeviceMemory> m_UniformBufferMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet; //gets automatically cleaned up with pool

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
