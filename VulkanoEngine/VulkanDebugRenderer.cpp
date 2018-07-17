#include "stdafx.h"
#include "VulkanDebugRenderer.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "Debug.h"
#include "GameContext.h"
#include "VulkanUtils.h"
#include "VkDebugPipeline_Ext.h"
#include "VkPhysicalDevice_Ext.h"
#include "GameBase.h"
#include "VkPipelineManager.h"
#include "GameScene.h"

VulkanDebugRenderer::VulkanDebugRenderer(VulkanContext* pVkContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize)
{
	auto debugGraphicsPipeline = VkPipelineManager::GetInstance()->GetDebugPipeline();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext, fixedLineList, bufferSize, fixedBufferSize);
	m_DescriptorPool = debugGraphicsPipeline->CreateDescriptorPool(*pVkContext->GetVkDevice());
	m_DescriptorSet = debugGraphicsPipeline->CreateAndWriteDescriptorSet(*pVkContext->GetVkDevice(), *m_DescriptorPool, *m_UniformBuffer);

}

VulkanDebugRenderer::~VulkanDebugRenderer()
{
}

void VulkanDebugRenderer::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, unsigned int vertexCount)
{
	auto debugGraphicsPipeline = VkPipelineManager::GetInstance()->GetDebugPipeline();
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *debugGraphicsPipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, debugGraphicsPipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
	//draw
	vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

}

void VulkanDebugRenderer::UpdateUniformVariables(const VulkanContext* pVkContext, const GameScene* pCurrentScene)
{
	//with camera from context
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetDebugPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

																	 //correct aligment and structure for opengl 
	ubo.wvp = mat4() * pCurrentScene->GetCamera()->GetViewProjection();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBufferMemory);
}

void VulkanDebugRenderer::UpdateVertexData(const VulkanContext* pVkContext, const vector<VertexPosCol>& lineList, unsigned int fixedBufferSize)
{
	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_VertexBufferMemory, fixedBufferSize*sizeof(lineList[0]), sizeof(lineList[0]) * lineList.size(), 0, &data); // VK_WHOLE_SIZE
	memcpy(data, &lineList[0], sizeof(lineList[0]) * lineList.size());
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_VertexBufferMemory);
}

void VulkanDebugRenderer::CreateVertexBuffer(VulkanContext* pVkContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize)
{
	auto device = *pVkContext->GetVkDevice();

	if(m_VertexBuffer!=nullptr)
		vkDeviceWaitIdle(device); //if old buffer object needs to be deleted wait for all async processes to complete, could be improved to wait 'till buffer is not in use
	m_VertexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
	m_VertexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
	VulkanUtils::CreateBuffer(pVkContext, sizeof(fixedLineList[0]) * (bufferSize+ fixedBufferSize)
		, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, m_VertexBuffer.get(), m_VertexBufferMemory.get());
	
	if (fixedBufferSize > 0)
	{
		void* data;
		vkMapMemory(device, *m_VertexBufferMemory, 0, (VkDeviceSize)(sizeof(fixedLineList[0])*fixedBufferSize), 0, &data);
		memcpy(data, &fixedLineList[0], sizeof(fixedLineList[0]) *(size_t)fixedBufferSize);
		vkUnmapMemory(device, *m_VertexBufferMemory);
	}

	pVkContext->SetFlags(VkContextFlags::InvalidDrawCommandBuffers);
}


void VulkanDebugRenderer::CreateUniformBuffer(const VulkanContext* pVkContext)
{
	auto device = *pVkContext->GetVkDevice();
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetDebugPipeline())::UniformBufferObject);
	m_UniformBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
	m_UniformBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
	VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer.get(), m_UniformBufferMemory.get());
}