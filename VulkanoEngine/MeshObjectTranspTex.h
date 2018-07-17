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

class MeshObjectTranspTex : public GameObject
{
public:
	MeshObjectTranspTex(wstring assetFile, wstring textureFile);
	~MeshObjectTranspTex(void) {};

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void CreateTextureResources(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer) override;

	shared_ptr<MeshData> m_pMeshData;
	wstring m_AssetFile;
	wstring  m_TextureFile;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	unique_ptr_del<VkBuffer> m_UniformBuffer;
	unique_ptr_del<VkDeviceMemory> m_UniformBufferMemory;
	unique_ptr_del<VkBasicSampler_Ext> m_TextureSampler;
	shared_ptr<VkTextureImage_Ext> m_TextureImage;
	unique_ptr_del<VkImageView_Ext> m_TextureImageView;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet; //gets automatically cleaned up with pool

	bool m_UseCustomColor = false;
	vec4 m_CustomColor;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshObjectTranspTex(const MeshObjectTranspTex& t);
	MeshObjectTranspTex& operator=(const MeshObjectTranspTex& t);
};
