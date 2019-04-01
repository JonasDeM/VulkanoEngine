#pragma once

#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include "VertexStructs.h"
#include <vector>


template<class T>
class HoldingContentLoader;
class VulkanContext;

class VkPipeline_Ext : public DeriveableHandle<VkPipeline>
{

public:
	// Exception! Extended Handle where the vulkan object isn't created in the constructor
	// Because I wanted to use the Template Method design pattern for creation, and this is not possible in the constructor
	void Build(VulkanContext* pVkContext, HoldingContentLoader<VkShaderModule>& shaderModuleLoader, VkPipeline basePipeline=VK_NULL_HANDLE);
	void Destroy(VkDevice device)
	{
		vkDestroyPipeline(device, *this, nullptr);
	}

	virtual unique_ptr_del<VkDescriptorPool> CreateDescriptorPool(VkDevice device, const int uboCount) const = 0;
	VkPipelineLayout GetPipelineLayout() const { return *m_pPipelineLayout; }

	// TODO: Make private for template methods
protected:
	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const = 0;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const = 0;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateCreateInfo() const;
	virtual void CreateViewportsAndScissors(VkExtent2D windowExtent, std::vector<VkViewport>& viewPortsToFill, std::vector<VkRect2D>& scissorsToFill) const;
	VkPipelineViewportStateCreateInfo CreateViewportStateCreateInfo(const std::vector<VkViewport>& viewPorts, const std::vector<VkRect2D>& scissors) const;
	virtual VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo() const;
	virtual VkPipelineMultisampleStateCreateInfo CreateMultisampleStateCreateInfo() const;
	virtual VkPipelineDepthStencilStateCreateInfo CreateDepthStencilStateCreateInfo() const;
	virtual std::unique_ptr<std::vector<VkPipelineColorBlendAttachmentState>> CreateColorBlendState(VkPipelineColorBlendStateCreateInfo& createInfoToFill) const;
	virtual std::unique_ptr<std::vector<VkDynamicState>> CreateDynamicState(VkPipelineDynamicStateCreateInfo& createInfoToFill) const;
	virtual VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device) const = 0;
	virtual VkPipelineLayout CreatePipelineLayout(VkDevice device) const;

	VkDescriptorSetLayout GetDescriptorSetLayout() const { return *m_pDescriptorSetLayout; }

private: 
	unique_ptr_del<VkPipelineLayout> m_pPipelineLayout;
	unique_ptr_del<VkDescriptorSetLayout> m_pDescriptorSetLayout;


};
