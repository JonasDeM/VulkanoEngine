#pragma once
#include "VkPipeline_Ext.h"

class VulkanContext;

class VkBasicGeometryPipeline_Ext : public VkPipeline_Ext
{
public:
	using VertexType = VertexPosColNorm; // each derived pipeline that implements it's own vertexInputState should typedef the used Vertex struct as VertexType like this

	unique_ptr_del<VkDescriptorPool> CreateDescriptorPool(VkDevice device, const int uboCount) const final override;
	std::vector<VkDescriptorSet> CreateAndWriteDescriptorSets(VkDevice device, VkDescriptorPool descPool, const vector<unique_ptr_del<VkBuffer>>& uniformBuffers) const;

	struct UniformBufferObject {
		glm::mat4 world;
		glm::mat4 wvp;
	};
protected:

	virtual std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const override;
	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo() const override;
	virtual VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device) const final override;
};
