
#include "stdafx.h"
#include "VkPosColNormPipeline_Ext.h"

#include <stdexcept>
#include <vulkan/vulkan.h>
#include "FileUtils.h"
#include <vector>
#include <array>
#include "VertexStructs.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VkSwapchainKHR_Ext.h"

VkPosColNormPipeline_Ext::VkPosColNormPipeline_Ext(VulkanContext* pVkContext)
{
	const VkDevice device = *pVkContext->GetVkDevice();

	//SHADERS
	auto vertShaderCode = FileUtils::ReadFile("./Shaders/poscolnormVert.spv");
	auto fragShaderCode = FileUtils::ReadFile("./Shaders/poscolnormFrag.spv");

	//need to be cleaned up at the end
	VkShaderModule vertShaderModule;
	CreateShaderModule(vertShaderCode, vertShaderModule, device);
	VkShaderModule fragShaderModule;
	CreateShaderModule(fragShaderCode, fragShaderModule, device);


	VkPipelineShaderStageCreateInfo vertStageCreateInfo = {};
	vertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageCreateInfo.module = vertShaderModule;
	vertStageCreateInfo.pName = "main"; //entry point

										// optional pSpecializationInfo ::  It allows you to specify values for shader constants. 
										//You can use a single shader module where its behavior can be configured at pipeline creation
										//by specifying different values for the constants used in it.

	VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
	fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageCreateInfo.module = fragShaderModule;
	fragStageCreateInfo.pName = "main"; //entry point

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageCreateInfo, fragStageCreateInfo };

	//Vertexinput
	auto bindingDescription = VertexPosColNorm::GetBindingDescription();
	auto attributeDescriptions = VertexPosColNorm::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	//vertexInputInfo.vertexBindingDescriptionCount = 0;
	//vertexInputInfo.pVertexBindingDescriptions = nullptr; // spacing between data and whether the data is per-vertex or per-instance
	//vertexInputInfo.vertexAttributeDescriptionCount = 0;
	//vertexInputInfo.pVertexAttributeDescriptions = nullptr; // [pos,norm,col,uv,..]

															// INPUT ASSEMBLY
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//viewport describes what part of the framebuffer that the output will be rendered to
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	auto swapChainExtent = pVkContext->GetVkSwapChain()->GetExtent();
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//can cut off a part of the viewport if it's smaller than the viewport
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	//It is possible to use multiple viewports and scissor rectangles on some graphics cards
	//so its members reference an array of them.
	//Using multiple requires enabling a GPU feature(see logical device creation).
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//RASTERIZER, takes geometry and rasterizes it
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE; //requires gpu feature, instead of discarding objects out of near and far plane, it clamps them (can be used for shadowmaps)
	rasterizer.rasterizerDiscardEnable = VK_FALSE; //disables output to framebuffer
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //can be wireframe for example, Using any mode other than fill requires enabling a GPU feature.
	rasterizer.lineWidth = 1.0f; // bigger than 1 needs a gpu feature
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

											//MULTISAMPLING, can be used for anti aliasing
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//depthstencil
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};

	//COLOR BLENDING, attachement state is per framebuffer, create info is global
	//this is the first way of blending, the other one is the logicOp variable in the global options
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	//can mask colors after blending
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	// these define alphablending
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; //does nothing bcuz not enabled
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	//blendConstants is an array of four values used as the R, G, B, and A components
	//of the blend constant that are used in blendingdepending on the blend factor.
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	//variables that can be changed at drawtime
	//This will cause the configuration of these values to be ignored and you will be required to specify the data at drawing time.
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	//depth test
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional


	//Pipelinelayout describes the 'uniform' variables used in shader (glsl uniform keyword, kind off shader global defined by ou program)
	// also push constants, which are another way of passing dynamic values to shaders
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	CreateDescriptorSetLayout(device);
	VkDescriptorSetLayout setLayouts[1] = { *m_DescriptorSetLayout };
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = setLayouts; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	m_PipelineLayout = CreateHandle<VkPipelineLayout>(vkDestroyPipelineLayout, device);

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
		m_PipelineLayout.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	//FINALLY CREATE THE PIPELINE
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = &depthStencil;
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = *m_PipelineLayout;
	pipelineCreateInfo.renderPass = pVkContext->GetVkSwapChain()->GetRenderPass();
	pipelineCreateInfo.subpass = 0;
	//to create a derivative from a base pipeline, needs the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineCreateInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, this->BasePtr()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
}


unique_ptr_del<VkDescriptorPool> VkPosColNormPipeline_Ext::CreateDescriptorPool(VkDevice device)
{
	std::array<VkDescriptorPoolSize, 1> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	auto pVkDescriptorPool = CreateHandle<VkDescriptorPool>(vkDestroyDescriptorPool, device);
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, pVkDescriptorPool.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
	return std::move(pVkDescriptorPool);
}

VkDescriptorSet VkPosColNormPipeline_Ext::CreateAndWriteDescriptorSet(VkDevice device, VkDescriptorPool descPool, VkBuffer uniformBuffer)
{
	VkDescriptorSet descSet;

	VkDescriptorSetLayout layouts[] = { *m_DescriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(device, &allocInfo, &descSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	return descSet;
}


void VkPosColNormPipeline_Ext::CreateDescriptorSetLayout(VkDevice device)
{
	//uniform variable like the marices to transform to screen space
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional


	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	m_DescriptorSetLayout = CreateHandle<VkDescriptorSetLayout>(vkDestroyDescriptorSetLayout, device);

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, m_DescriptorSetLayout.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

