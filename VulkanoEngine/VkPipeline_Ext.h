#pragma once

#include "DeriveableHandle.h"
#include "HandleUtilities.h"

class VkPipeline_Ext : public DeriveableHandle<VkPipeline>
{
public:
	VkPipeline_Ext() {};
	virtual void Destroy(VkDevice device) const = 0;

	virtual void CreateDescriptorSetLayout(VkDevice device) = 0;


	virtual unique_ptr_del<VkDescriptorPool> CreateDescriptorPool(VkDevice device) = 0;

	VkPipelineLayout GetPipelineLayout() { return *m_PipelineLayout; }

protected:
	static void CreateShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule, VkDevice device);

	unique_ptr_del<VkDescriptorSetLayout> m_DescriptorSetLayout;
	unique_ptr_del<VkPipelineLayout> m_PipelineLayout;
};

