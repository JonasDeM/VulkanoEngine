#pragma once
#pragma once
#include "stdafx.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "PipelineManager.h"
#include "MeshData.h"
#include "VulkanContext.h"
#include "GameScene.h"
#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkDevice_Ext.h"
#include "VkSwapchainKHR_Ext.h"
#include "VkBasicSampler_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkImageView_Ext.h"

template<class Pipeline>
class GeometryObjectTex final : public GameObject
{
	static_assert(std::is_base_of<VkPipeline_Ext, Pipeline>::value, "Template parameter Pipeline must derive from VkPipeline_Ext");
public:
	GeometryObjectTex(wstring assetFile, wstring texFile, bool isStatic = false);
	virtual ~GeometryObjectTex(void) = default;
	GeometryObjectTex(const GeometryObjectTex& t) = delete;
	GeometryObjectTex& operator=(const GeometryObjectTex& t) = delete;

	void Initialize(VulkanContext* pVkContext) override;
	void Update(VulkanContext* pVkContext) override;

protected:
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;
private:
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void CreateTextureResources(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);

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
};


template<class Pipeline>
GeometryObjectTex<Pipeline>::GeometryObjectTex(wstring assetFile, wstring texFile, bool isStatic) :
	GameObject(isStatic),
	m_AssetFile(assetFile),
	m_TextureFile(texFile)
{
}

template<class Pipeline>
void GeometryObjectTex<Pipeline>::Initialize(VulkanContext * pVkContext)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<Pipeline::VertexType>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	CreateTextureResources(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers, *m_TextureImageView, *m_TextureSampler);
}

template<class Pipeline>
void GeometryObjectTex<Pipeline>::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}

template<class Pipeline>
void GeometryObjectTex<Pipeline>::UpdateUniformVariables(VulkanContext* pVkContext)
{
	Pipeline::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentFrameIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}

template<class Pipeline>
void GeometryObjectTex<Pipeline>::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_pMeshData->GetIndexCount()), 1, 0, 0, 0);
}

//per scene object
template<class Pipeline>
void GeometryObjectTex<Pipeline>::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(Pipeline::UniformBufferObject);
	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}

template<class Pipeline>
void GeometryObjectTex<Pipeline>::CreateTextureResources(VulkanContext* pVkContext)
{
	m_TextureSampler = CreateExtendedHandle(new VkBasicSampler_Ext(*pVkContext->GetVkDevice()), *pVkContext->GetVkDevice());
	m_TextureImage = ContentManager::Load<VkTextureImage_Ext>(m_TextureFile.c_str());
	m_TextureImageView = CreateExtendedHandle(new VkImageView_Ext(*pVkContext->GetVkDevice(), *m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT), *pVkContext->GetVkDevice());
}