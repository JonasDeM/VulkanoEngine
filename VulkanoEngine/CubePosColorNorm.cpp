#include "stdafx.h"
#include "CubePosColorNorm.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkPipelineManager.h"
#include "VulkanContext.h"
#include "GameScene.h"

CubePosColorNorm::CubePosColorNorm(float width, float height, float depth) :
	m_Width(width),
	m_Height(height),
	m_Depth(depth)
{
}

CubePosColorNorm::CubePosColorNorm(float width, float height, float depth, vec4 color) :
	CubePosColorNorm(width, height, depth)
{
	m_UseCustomColor = true;
	m_CustomColor = color;
}

CubePosColorNorm::~CubePosColorNorm(void)
{
}

void CubePosColorNorm::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext);
	CreateIndexBuffer(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers);
}

void CubePosColorNorm::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}

void CubePosColorNorm::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that hraphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world;

	int i = pVkContext->GetCurrentDrawingBufferIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}


void CubePosColorNorm::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColNormPipeline();

	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

	VkBuffer vertexBuffers[] = { *m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(cmdBuffer, *m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[frameBufferIndex], 0, nullptr);
	//draw
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(NUMINDICES), 1, 0, 0, 0);

}

//per scene object
void CubePosColorNorm::CreateIndexBuffer(VulkanContext* pVkContext)
{
	//Indexbuffer
	uint16_t indices[NUMINDICES] = {
		0,1,2, 2,1,3,
		4,6,5, 5,6,7,
		8,10,9, 9,10,11,
		12,13,14, 14,13,15,
		16,18,17, 17,18,19,
		20,21,22, 22,21,23
	};
	VkDeviceSize bufferSize = sizeof(indices[0]) * NUMINDICES;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, &stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &indices[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory);


	m_IndexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_IndexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_IndexBuffer.get(), m_IndexBufferMemory.get());

	VulkanUtils::CopyBuffer(pVkContext, stagingBuffer, *m_IndexBuffer, bufferSize);

	vkDestroyBuffer(*pVkContext->GetVkDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, nullptr);
}
	
//per scene object
void CubePosColorNorm::CreateVertexBuffer(VulkanContext* pVkContext)
{
	//Vertex Buffer
	VertexPosColNorm vertices[NUMVERTICES] = {};
	int index = 0;

	//keep vulkan coordinate system in mind
	//front red
	vec4 color = m_UseCustomColor ? m_CustomColor : vec4(1, 0, 0, 1);
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, -1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, -1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, -1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, -1);
	vertices[index++].Color = color;
	//back red
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 0, 1);
	vertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 1, 0, 1);
	//left green
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index++].Color = color;

	//right green
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(1, 0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(1, 0, 0);
	vertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 0, 1, 1);
	//top blue
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, 1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, 1, 0);
	vertices[index++].Color = color;

	//bottom blue
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, -1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, -1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	vertices[index].Normal = vec3(0, -1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	vertices[index].Normal = vec3(0, -1, 0);
	vertices[index++].Color = color;


	//todo use my handles instead of manual cleanup
	VkDeviceSize bufferSize = sizeof(vertices[0]) * NUMVERTICES;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, &stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, &vertices[0], (size_t)bufferSize);
	vkUnmapMemory(*pVkContext->GetVkDevice(), stagingBufferMemory);


	m_VertexBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
	m_VertexBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
	VulkanUtils::CreateBuffer(pVkContext, bufferSize
		, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		, m_VertexBuffer.get(), m_VertexBufferMemory.get());

	VulkanUtils::CopyBuffer(pVkContext, stagingBuffer, *m_VertexBuffer, bufferSize);

	vkDestroyBuffer(*pVkContext->GetVkDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*pVkContext->GetVkDevice(), stagingBufferMemory, nullptr);
}

//per scene object
void CubePosColorNorm::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColNormPipeline())::UniformBufferObject);

	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}