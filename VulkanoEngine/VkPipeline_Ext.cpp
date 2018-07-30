#include "stdafx.h"
#include "VkPipeline_Ext.h"
#include <array>
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VkSwapchainKHR_Ext.h"

void VkPipeline_Ext::Build(VulkanContext * pVkContext, HoldingContentLoader<VkShaderModule>& shaderModuleLoader, VkPipeline basePipeline)
{
	const VkDevice device = *pVkContext->GetVkDevice();
	
	//Creation of parts is implemented by inherited classes
	std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos
		= CreateShaderStageCreateInfos(shaderModuleLoader);
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = CreateVertexInputStateCreateInfo();
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreateInputAssemblyStateCreateInfo();
	std::vector<VkViewport> viewPorts{};
	std::vector<VkRect2D> scissors{};
	CreateViewportsAndScissors(pVkContext->GetVkSwapChain()->GetExtent(), viewPorts, scissors);
	VkPipelineViewportStateCreateInfo viewportState = CreateViewportStateCreateInfo(viewPorts, scissors);
	VkPipelineRasterizationStateCreateInfo rasterizer = CreateRasterizationStateCreateInfo();
	VkPipelineMultisampleStateCreateInfo multisampling = CreateMultisampleStateCreateInfo();
	VkPipelineColorBlendStateCreateInfo colorBlendingInfo = {};
	auto colorBlendingState = CreateColorBlendState(colorBlendingInfo);
	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	auto dynamicStates = CreateDynamicState(dynamicStateInfo);
	VkPipelineDepthStencilStateCreateInfo depthStencil = CreateDepthStencilStateCreateInfo();

	m_pDescriptorSetLayout = CreateHandle<VkDescriptorSetLayout>(vkDestroyDescriptorSetLayout, device);
	*m_pDescriptorSetLayout = CreateDescriptorSetLayout(device);
	m_pPipelineLayout = CreateHandle<VkPipelineLayout>(vkDestroyPipelineLayout, device);
	*m_pPipelineLayout = CreatePipelineLayout(device);

	//FINALLY CREATE THE PIPELINE
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.flags = basePipeline == VK_NULL_HANDLE ? 0 : VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	pipelineCreateInfo.stageCount = (uint32_t)pipelineShaderStageCreateInfos.size();
	pipelineCreateInfo.pStages = pipelineShaderStageCreateInfos.data();
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = &depthStencil;
	pipelineCreateInfo.pColorBlendState = &colorBlendingInfo;
	if (dynamicStates)
		pipelineCreateInfo.pDynamicState = &dynamicStateInfo;
	else 
		pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = *m_pPipelineLayout;
	pipelineCreateInfo.renderPass = pVkContext->GetVkSwapChain()->GetRenderPass();
	pipelineCreateInfo.subpass = 0;
	//to create a derivative from a base pipeline, needs the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag
	pipelineCreateInfo.basePipelineHandle = basePipeline; // Optional
	pipelineCreateInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, this->BasePtr()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}


VkPipelineInputAssemblyStateCreateInfo VkPipeline_Ext::CreateInputAssemblyStateCreateInfo() const
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

void VkPipeline_Ext::CreateViewportsAndScissors(VkExtent2D windowExtent, std::vector<VkViewport>& viewPortsToFill, std::vector<VkRect2D>& scissorsToFill) const
{
	//viewport describes what part of the framebuffer that the output will be rendered to
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(windowExtent.width);
	viewport.height = static_cast<float>(windowExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	viewPortsToFill.emplace_back(viewport);

	//can cut off a part of the viewport if it's smaller than the viewport
	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = windowExtent;

	scissorsToFill.emplace_back(scissor);
}

VkPipelineViewportStateCreateInfo VkPipeline_Ext::CreateViewportStateCreateInfo(const std::vector<VkViewport>& viewPorts, const std::vector<VkRect2D>& scissors) const
{
	//It is possible to use multiple viewports and scissor rectangles on some graphics cards
	//so its members reference an array of them.
	//Using multiple requires enabling a GPU feature(see logical device creation).
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = (uint32_t)viewPorts.size();
	viewportState.pViewports = viewPorts.data();
	viewportState.scissorCount = (uint32_t)scissors.size();
	viewportState.pScissors = scissors.data();

	return viewportState;
}

VkPipelineRasterizationStateCreateInfo VkPipeline_Ext::CreateRasterizationStateCreateInfo() const
{
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

	return rasterizer;
}

VkPipelineMultisampleStateCreateInfo VkPipeline_Ext::CreateMultisampleStateCreateInfo() const
{
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;
	return multisampling;
}

VkPipelineDepthStencilStateCreateInfo VkPipeline_Ext::CreateDepthStencilStateCreateInfo() const
{
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
	return depthStencil;
}

std::unique_ptr<std::vector<VkPipelineColorBlendAttachmentState>> VkPipeline_Ext::CreateColorBlendState(VkPipelineColorBlendStateCreateInfo &createInfoToFill) const
{
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

	auto attachments
		= std::make_unique<std::vector<VkPipelineColorBlendAttachmentState>>(std::vector<VkPipelineColorBlendAttachmentState>{std::move(colorBlendAttachment)});

	createInfoToFill.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	createInfoToFill.logicOpEnable = VK_FALSE;
	createInfoToFill.logicOp = VK_LOGIC_OP_COPY; //does nothing bcuz not enabled
	createInfoToFill.attachmentCount = (uint32_t)attachments->size();
	createInfoToFill.pAttachments = attachments->data();
	//blendConstants is an array of four values used as the R, G, B, and A components
	//of the blend constant that are used in blendingdepending on the blend factor.
	createInfoToFill.blendConstants[0] = 0.0f;
	createInfoToFill.blendConstants[1] = 0.0f;
	createInfoToFill.blendConstants[2] = 0.0f;
	createInfoToFill.blendConstants[3] = 0.0f;

	return attachments;
}

std::unique_ptr<std::vector<VkDynamicState>> VkPipeline_Ext::CreateDynamicState(VkPipelineDynamicStateCreateInfo& createInfoToFill) const
{
	return nullptr;
}

VkPipelineLayout VkPipeline_Ext::CreatePipelineLayout(VkDevice device) const
{
	//Pipelinelayout describes the 'uniform' variables used in shader (glsl uniform keyword, kind off shader global defined by ou program)
	// also push constants, which are another way of passing dynamic values to shaders
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	VkDescriptorSetLayout setLayouts[1] = { GetDescriptorSetLayout() };
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = setLayouts; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
		&layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
	return layout;
}