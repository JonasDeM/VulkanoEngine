#pragma once
#include "Component.h"
#include "HandleUtilities.h"
#include "MeshData.h"
#include <type_traits>
#include <memory>
#include <string>
#include <vector>
#include "VkPosNormTexPipeline_Ext.h"
#include "VkPosNormTex2SPipeline_Ext.h"

class VkBasicSampler_Ext;
class VkTextureImage_Ext;
class VkImageView_Ext;
class TransformComponent;

class TexMeshComponent :public Component
{
public:
	TexMeshComponent(wstring assetFile, wstring texFile, bool twoSided);
	~TexMeshComponent() = default;
	TexMeshComponent(const TexMeshComponent& t) = delete;
	TexMeshComponent& operator=(const TexMeshComponent& t) = delete;

private:
	void Build(VulkanContext* vkContext) override;
	void Update(VulkanContext* vkContext) override;
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;

	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void CreateTextureResources(VulkanContext* pVkContext);

	TransformComponent* m_pTransform;

	std::shared_ptr<MeshData> m_pMeshData;
	std::wstring m_AssetFile;
	std::wstring m_TexFile;
	using Pipeline = VkPosNormTexPipeline_Ext;
	using Pipeline2S = VkPosNormTex2SPipeline_Ext;
	Pipeline* m_MeshPipeline;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;
	unique_ptr_del<VkBasicSampler_Ext> m_TextureSampler;
	std::shared_ptr<VkTextureImage_Ext> m_TextureImage;
	unique_ptr_del<VkImageView_Ext> m_TextureImageView;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool
};
