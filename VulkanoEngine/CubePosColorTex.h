#pragma once
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "VkPosColTexPipeline_Ext.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkImageView_Ext.h"

class VulkanContext;

class CubePosColorTex : public GameObject
{
public:
	CubePosColorTex(float width, float height, float depth, wstring tex= L"Textures/texture.jpg");
	CubePosColorTex(float width, float height, float depth, vec4 color, wstring tex = L"Textures/texture.jpg");
	~CubePosColorTex(void);

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateTextureResources(VulkanContext* pVkContext);
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
	unique_ptr_del<VkBasicSampler_Ext> m_TextureSampler;
	std::shared_ptr<VkTextureImage_Ext> m_TextureImage;
	unique_ptr_del<VkImageView_Ext> m_TextureImageView;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

	bool m_UseCustomColor = false;
	vec4 m_CustomColor;
	wstring m_TextureFile;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	CubePosColorTex(const CubePosColorTex& t);
	CubePosColorTex& operator=(const CubePosColorTex& t);
};
