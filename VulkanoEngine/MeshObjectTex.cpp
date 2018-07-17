#include "stdafx.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkImageView_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkPipelineManager.h"
#include "MeshData.h"
#include "VkTextureLoader.h"
#include "MeshObjectTex.h"
#include "VulkanContext.h"
#include "GameScene.h"

MeshObjectTex::MeshObjectTex(wstring assetFile, wstring textureFile) :
	m_AssetFile(assetFile),
	m_TextureFile(textureFile)
{
}

void MeshObjectTex::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosNormTexPipeline();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::GetInstance()->Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<VertexPosNormTex>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	CreateTextureResources(pVkContext);
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice());
	m_DescriptorSet = pipeline->CreateAndWriteDescriptorSet(*pVkContext->GetVkDevice(), *m_DescriptorPool, *m_UniformBuffer, *m_TextureImageView, *m_TextureSampler);

	//Debug::Log(L"Failed to Create IndexBuffer");
}

void MeshObjectTex::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}


void MeshObjectTex::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosNormTexPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory);
}


void MeshObjectTex::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosNormTexPipeline();
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_pMeshData->GetIndexCount()), 1, 0, 0, 0);

}


void MeshObjectTex::CreateTextureResources(VulkanContext* pVkContext)
{
	m_TextureSampler = CreateExtendedHandle(new VkBasicSampler_Ext(*pVkContext->GetVkDevice()), *pVkContext->GetVkDevice());
	m_TextureImage = ContentManager::GetInstance()->LoadVk<VkTextureImage_Ext>(m_TextureFile.c_str(), pVkContext);
	m_TextureImageView = CreateExtendedHandle(new VkImageView_Ext(*pVkContext->GetVkDevice(), *m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT), *pVkContext->GetVkDevice());
}

//per scene object
void MeshObjectTex::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosNormTexPipeline())::UniformBufferObject);
	m_UniformBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_UniformBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer.get(), m_UniformBufferMemory.get());
}