#pragma once
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
#include "PipelineManager.h"
#include "VkSwapChainKHR_Ext.h"
#include "GameScene.h"

VulkanDebugRenderer::VulkanDebugRenderer(VulkanContext* pVkContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize)
{
	auto debugGraphicsPipeline = PipelineManager::GetPipeline<VkDebugPipeline_Ext>();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext, fixedLineList, bufferSize, fixedBufferSize);
	m_DescriptorPool = debugGraphicsPipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), pVkContext->GetVkSwapChain()->GetAmountImages());
	m_DescriptorSets = debugGraphicsPipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

VulkanDebugRenderer::~VulkanDebugRenderer()
{
}

void VulkanDebugRenderer::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, unsigned int vertexCount, const int frameBufferIndex)
{
	auto debugGraphicsPipeline = PipelineManager::GetPipeline<VkDebugPipeline_Ext>();
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *debugGraphicsPipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffers[frameBufferIndex] };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, debugGraphicsPipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

}

void VulkanDebugRenderer::UpdateUniformVariables(const VulkanContext* pVkContext, const GameScene* pCurrentScene)
{
	//with camera from context
	GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkDebugPipeline_Ext>())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.wvp = mat4() * pCurrentScene->GetCamera()->GetViewProjection();

	int index = pVkContext->GetCurrentFrameIndex();
	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[index], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[index]);
}

void VulkanDebugRenderer::UpdateVertexData(const VulkanContext* pVkContext, const vector<VertexPosCol>& lineList, unsigned int fixedBufferSize)
{
	int index = pVkContext->GetCurrentFrameIndex();
	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_VertexBuffersMemory[index], fixedBufferSize*sizeof(lineList[0]), sizeof(lineList[0]) * lineList.size(), 0, &data); // VK_WHOLE_SIZE
	memcpy(data, &lineList[0], sizeof(lineList[0]) * lineList.size());
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_VertexBuffersMemory[index]);
}

void VulkanDebugRenderer::CreateVertexBuffer(VulkanContext* pVkContext, const vector<VertexPosCol>& fixedLineList, unsigned int bufferSize, unsigned int fixedBufferSize)
{
	auto device = *pVkContext->GetVkDevice();

	m_VertexBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_VertexBuffersMemory.resize(m_VertexBuffers.size());

	if(!m_VertexBuffers.empty())
		vkDeviceWaitIdle(device); //if old buffer object needs to be deleted(because new buffers get created) wait for all async processes to complete, could be improved to wait 'till buffer is not in use
	for (size_t i = 0; i < m_VertexBuffers.size(); i++)
	{
		m_VertexBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
		m_VertexBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
		VulkanUtils::CreateBuffer(pVkContext, sizeof(fixedLineList[0]) * (bufferSize + fixedBufferSize)
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, m_VertexBuffers[i].get(), m_VertexBuffersMemory[i].get());

	}
	if (fixedBufferSize > 0)
	{
		for (auto& mem : m_VertexBuffersMemory)
		{
			void* data;
			vkMapMemory(device, *mem, 0, (VkDeviceSize)(sizeof(fixedLineList[0])*fixedBufferSize), 0, &data);
			memcpy(data, &fixedLineList[0], sizeof(fixedLineList[0]) *(size_t)fixedBufferSize);
			vkUnmapMemory(device, *mem);
		}
	}

	pVkContext->SetFlags(VkContextFlags::InvalidDrawCommandBuffers);
}


void VulkanDebugRenderer::CreateUniformBuffer(const VulkanContext* pVkContext)
{
	auto device = *pVkContext->GetVkDevice();
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(PipelineManager::GetPipeline<VkDebugPipeline_Ext>())::UniformBufferObject);

	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}