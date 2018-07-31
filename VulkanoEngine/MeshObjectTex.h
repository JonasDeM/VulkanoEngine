#pragma once
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "VkPosNormTexPipeline_Ext.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkImageView_Ext.h"

#include <string>

class MeshData;

class MeshObjectTex : public GameObject
{
public:
	MeshObjectTex(wstring assetFile, wstring textureFile, bool isStatic=false);
	~MeshObjectTex(void) = default;
	MeshObjectTex(const MeshObjectTex& t) = delete;
	MeshObjectTex& operator=(const MeshObjectTex& t) = delete;

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateUniformBuffers(VulkanContext* pVkContext);
	void CreateTextureResources(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex) override;

	shared_ptr<MeshData> m_pMeshData;
	wstring m_AssetFile;
	wstring  m_TextureFile;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;
	unique_ptr_del<VkBasicSampler_Ext> m_TextureSampler;
	std::shared_ptr<VkTextureImage_Ext> m_TextureImage;
	unique_ptr_del<VkImageView_Ext> m_TextureImageView;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool

	bool m_UseCustomColor = false;
	vec4 m_CustomColor;
private:
};
