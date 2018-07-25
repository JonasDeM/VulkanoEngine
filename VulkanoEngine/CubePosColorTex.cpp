#include "stdafx.h"
#include "CubePosColorTex.h"
#include "VertexStructs.h"
#include "ContentManager.h"
#include "VulkanUtils.h"
#include "HandleUtilities.h"
#include "VkBasicSampler_Ext.h"
#include "VkImageView_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkPipelineManager.h"
#include "VulkanContext.h"
#include "GameScene.h"
#include "VkTextureImage_Ext.h"

CubePosColorTex::CubePosColorTex(float width, float height, float depth, wstring tex) :
	m_Width(width),
	m_Height(height),
	m_Depth(depth),
	m_TextureFile(tex)
{
}

CubePosColorTex::CubePosColorTex(float width, float height, float depth, vec4 color, wstring tex) :
	CubePosColorTex(width, height, depth, tex)
{
	m_UseCustomColor = true;
	m_CustomColor = color;
}

CubePosColorTex::~CubePosColorTex(void)
{
}

void CubePosColorTex::Initialize(VulkanContext* pVkContext)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColTexPipeline();
	CreateUniformBuffer(pVkContext);
	CreateVertexBuffer(pVkContext);
	CreateIndexBuffer(pVkContext);
	CreateTextureResources(pVkContext);
	int amountFrameBuffers = pVkContext->GetVkSwapChain()->GetAmountImages();
	m_DescriptorPool = pipeline->CreateDescriptorPool(*pVkContext->GetVkDevice(), amountFrameBuffers);
	m_DescriptorSets = pipeline->CreateAndWriteDescriptorSets(*pVkContext->GetVkDevice(), *m_DescriptorPool, m_UniformBuffers, *m_TextureImageView, *m_TextureSampler);
}

void CubePosColorTex::Update(VulkanContext* pVkContext)
{
	UpdateUniformVariables(pVkContext);
}

void CubePosColorTex::UpdateUniformVariables(VulkanContext* pVkContext)
{
	GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColTexPipeline())::UniformBufferObject ubo; // this way you can get the UniformBufferObject declared in that graphics pipeline

	ubo.world = m_WorldMatrix;
	ubo.wvp = GetScene()->GetCamera()->GetViewProjection() * ubo.world; 
	
	int i = pVkContext->GetCurrentDrawingBufferIndex();

	void* data;
	vkMapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*pVkContext->GetVkDevice(), *m_UniformBuffersMemory[i]);
}

void CubePosColorTex::RecordVulkanDrawCommands(VkCommandBuffer cmdBuffer, const int frameBufferIndex)
{
	auto pipeline = VkPipelineManager::GetInstance()->GetPosColTexPipeline();

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
void CubePosColorTex::CreateIndexBuffer(VulkanContext* pVkContext)
{
	uint16_t indices[NUMINDICES] = {
		2,1,0, 3,1,2,
		5,6,4, 7,6,5,
		9,10,8, 11,10,9,
		14,13,12, 15,13,14,
		17,18,16, 19,18,17,
		22,21,20, 23,21,22
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
void CubePosColorTex::CreateTextureResources(VulkanContext* pVkContext)
{
	m_TextureSampler = CreateExtendedHandle(new VkBasicSampler_Ext(*pVkContext->GetVkDevice()), *pVkContext->GetVkDevice());
	m_TextureImage = ContentManager::GetInstance()->LoadVk<VkTextureImage_Ext>(m_TextureFile, pVkContext);
	m_TextureImageView = CreateExtendedHandle(new VkImageView_Ext(*pVkContext->GetVkDevice(), *m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT), *pVkContext->GetVkDevice());
}

//per scene object
void CubePosColorTex::CreateVertexBuffer(VulkanContext* pVkContext)
{
	//Vertex Buffer
	VertexPosColTex vertices[NUMVERTICES] = {};
	int index = 0;

	//front red
	vec4 color = m_UseCustomColor ? m_CustomColor : vec4(0.5f, 0.5f, 0.5f, 1);
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, -1);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	////vertices[index].Normal = vec3(0, 0, -1);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	////vertices[index].Normal = vec3(0, 0, -1);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, -1);
	vertices[index].TexCoord = vec2(1, 1);
	vertices[index++].Color = color;
	//back red
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, 1);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, 1);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, 1);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 0, 1);
	vertices[index].TexCoord = vec2(1, 1);
	vertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 1, 0, 1);
	//left green
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(-1, 0, 0);
	vertices[index].TexCoord = vec2(1, 1);
	vertices[index++].Color = color;

	//right green
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(1, 0, 0);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(1, 0, 0);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(1, 0, 0);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(1, 0, 0);
	vertices[index].TexCoord = vec2(1, 1);
	vertices[index++].Color = color;

	color = m_UseCustomColor ? m_CustomColor : vec4(0, 0, 1, 1);
	//top blue
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 1, 0);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, 1, 0);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, 1, 0);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, 1, 0);
	vertices[index].TexCoord = vec2(1, 1);
	vertices[index++].Color = color;

	//bottom blue
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, -1, 0);
	vertices[index].TexCoord = vec2(0, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(-m_Width / 2, -m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, -1, 0);
	vertices[index].TexCoord = vec2(0, 1);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, m_Depth / 2);
	//vertices[index].Normal = vec3(0, -1, 0);
	vertices[index].TexCoord = vec2(1, 0);
	vertices[index++].Color = color;
	vertices[index].Position = vec3(m_Width / 2, -m_Height / 2, -m_Depth / 2);
	//vertices[index].Normal = vec3(0, -1, 0);
	vertices[index].TexCoord = vec2(1, 1);
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
void CubePosColorTex::CreateUniformBuffer(VulkanContext* pVkContext)
{
	VkDeviceSize bufferSize = sizeof(GET_CLASS_FROM_PTR(VkPipelineManager::GetInstance()->GetPosColTexPipeline())::UniformBufferObject);
	m_UniformBuffers.resize(pVkContext->GetVkSwapChain()->GetAmountImages());
	m_UniformBuffersMemory.resize(m_UniformBuffers.size());
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i] = CreateHandle<VkBuffer>(vkDestroyBuffer, *pVkContext->GetVkDevice());
		m_UniformBuffersMemory[i] = CreateHandle<VkDeviceMemory>(vkFreeMemory, *pVkContext->GetVkDevice());
		VulkanUtils::CreateBuffer(pVkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i].get(), m_UniformBuffersMemory[i].get());
	}
}