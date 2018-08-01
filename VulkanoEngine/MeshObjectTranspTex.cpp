#pragma once
#include "stdafx.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkImageView_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkPosNormTex2SPipeline_Ext.h"
#include "PipelineManager.h"
#include "MeshData.h"
#include "VkTextureLoader.h"
#include "MeshObjectTranspTex.h"
#include "VulkanContext.h"
#include "VkSwapchainKHR_Ext.h"
#include "GameScene.h"
#include "VkDevice_Ext.h"

MeshObjectTranspTex::MeshObjectTranspTex(wstring assetFile, wstring textureFile, bool isStatic) :
	GameObject(isStatic),
	m_AssetFile(assetFile),
	m_TextureFile(textureFile)
{
}

void MeshObjectTranspTex::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = PipelineManager::GetPipeline<VkPosNormTex2SPipeline_Ext>();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<VertexPosNormTex>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	CreateTextureResources(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers, *m_TextureImageView, *m_TextureSampler);
}

void MeshObjectTranspTex::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}


void MeshObjectTranspTex::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkPosNormTex2SPipeline_Ext>())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentFrameIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}


void MeshObjectTranspTex::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	auto pipeline = PipelineManager::GetPipeline<VkPosNormTex2SPipeline_Ext>();
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_pMeshData->GetIndexCount()), 1, 0, 0, 0);
}


void MeshObjectTranspTex::CreateTextureResources(VulkanContext* pVkContext)
{
	m_TextureSampler = CreateExtendedHandle(new VkBasicSampler_Ext(*pVkContext->GetVkDevice()), *pVkContext->GetVkDevice());
	m_TextureImage = ContentManager::Load<VkTextureImage_Ext>(m_TextureFile.c_str());
	m_TextureImageView = CreateExtendedHandle(new VkImageView_Ext(*pVkContext->GetVkDevice(), *m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT), *pVkContext->GetVkDevice());
}

//per scene object
void MeshObjectTranspTex::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkPosNormTex2SPipeline_Ext>())::UniformBufferObject);
	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}