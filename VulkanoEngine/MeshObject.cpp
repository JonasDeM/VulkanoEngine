#pragma once
#include "stdafx.h"
#include "MeshObject.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "PipelineManager.h"
#include "MeshData.h"
#include "VulkanContext.h"
#include "GameScene.h"
#include "VkSwapchainKHR_Ext.h"
#include "VkDevice_Ext.h"

MeshObject::MeshObject(wstring assetFile, bool isStatic) :
	GameObject(isStatic),
	m_pMeshData(nullptr),
	m_AssetFile(assetFile)
{
}

MeshObject::~MeshObject(void)
{
}

void MeshObject::Initialize(VulkanContext* pVkContext)
{
	auto pipeline =	PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<VertexPosColNorm>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

void MeshObject::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}


void MeshObject::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentFrameIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}


void MeshObject::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>();

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
void MeshObject::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkBasicGeometryPipeline_Ext>())::UniformBufferObject);
	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}