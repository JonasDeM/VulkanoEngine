#include "stdafx.h"
#include "MeshObject.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkPipelineManager.h"
#include "MeshData.h"
#include "VulkanContext.h"
#include "GameScene.h"

MeshObject::MeshObject(wstring assetFile) :
	m_pMeshData(nullptr),
	m_AssetFile(assetFile)
{
}

MeshObject::~MeshObject(void)
{
}

void MeshObject::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();
	CreateUniformBuffer(pVkContext);
	m_pMeshData = ContentManager::GetInstance()->Load<MeshData>(m_AssetFile);
	m_pVertexBuffer = m_pMeshData->GetVertexBuffer<VertexPosColNorm>(pVkContext);
	m_pIndexBuffer = m_pMeshData->GetIndexBuffer(pVkContext);
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice());
	m_DescriptorSet = pipeline->CreateAndWriteDescriptorSet(*pVkContext->GetVkDevice(), *m_DescriptorPool, *m_UniformBuffer);
}

void MeshObject::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}


void MeshObject::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory);
}


void MeshObject::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_pVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_pIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_pMeshData.get()->GetIndexCount()), 1, 0, 0, 0);
}

//per scene object
void MeshObject::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject);
	m_UniformBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_UniformBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer.get(), m_UniformBufferMemory.get());
}