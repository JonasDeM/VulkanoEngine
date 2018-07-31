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
#include "VkBasicGeometryPipeline_Ext.h"

#include "GameObject.h"
#include <glm\glm.hpp>
#include "VkPhysicalDevice_Ext.h"
#include "HandleUtilities.h"
#include "VkImageView_Ext.h"


class MeshData;

template<class Pipeline>
class GeometryObject : public GameObject
{
	static_assert(std::is_base_of<VkPipeline_Ext, Pipeline>::value, "Template parameter Pipeline must derive from VkPipeline_Ext");
public:
	GeometryObject(wstring assetFile, bool isStatic = false);
	virtual ~GeometryObject(void) = default;
	GeometryObject(const GeometryObject& t) = delete;
	GeometryObject& operator=(const GeometryObject& t) = delete;

	virtual void Initialize(VulkanContext* pVkContext) override;
	virtual void Update(VulkanContext* pVkContext) override;

private:
	void CreateUniformBuffer(VulkanContext* pVkContext);
	void UpdateUniformVariables(VulkanContext* pVkContext);
	void RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex) override;

	shared_ptr<MeshData> m_pMeshData;
	wstring m_AssetFile;

	VkBuffer *m_pVertexBuffer, *m_pIndexBuffer; //normal ptr -> no ownership
	std::vector<unique_ptr_del<VkBuffer>> m_UniformBuffers;
	std::vector<unique_ptr_del<VkDeviceMemory>> m_UniformBuffersMemory;

	unique_ptr_del<VkDescriptorPool> m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets; //gets automatically cleaned up with pool
};


template<class Pipeline>
GeometryObject<Pipeline>::GeometryObject(wstring assetFile, bool isStatic) :
	GameObject(isStatic),
	m_AssetFile(assetFile)
{
}

template<class Pipeline>
void GeometryObject<Pipeline>::Initialize(VulkanContext * pVkContext)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<Pipeline::VertexType>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

template<class Pipeline>
void GeometryObject<Pipeline>::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}

template<class Pipeline>
void GeometryObject<Pipeline>::UpdateUniformVariables(VulkanContext* pVkContext)
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
void GeometryObject<Pipeline>::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<Pipeline>();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_pMeshData.get()->GetIndexCount()), 1, 0, 0, 0);
}

//per scene object
template<class Pipeline>
void GeometryObject<Pipeline>::CreateUniformBuffer(VulkanContext* pVkContext)
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
