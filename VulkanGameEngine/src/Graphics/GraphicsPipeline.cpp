#include "GraphicsPipeline.h"

#include <stdexcept>

#include "Vertex.h"

// Dynamic States
const std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

// Constructor
GraphicsPipeline::GraphicsPipeline(Device* device, Swapchain* swapchain, RenderPass* renderPass)
: m_Device(device), m_Swapchain(swapchain), m_RenderPass(renderPass) {
	// Create shaders
	Shader vertShader(m_Device, VK_SHADER_STAGE_VERTEX_BIT, "src/res/shaders/default_vert.spv");
	Shader fragShader(m_Device, VK_SHADER_STAGE_FRAGMENT_BIT, "src/res/shaders/default_frag.spv");

	// Add shaders to pipeline
	AddShader(&vertShader);
	AddShader(&fragShader);

	// Pipeline vertex input info
	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	// Input assembly info
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport details
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_Swapchain->GetExtent().width;
	viewport.height = (float)m_Swapchain->GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Create scissor
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Swapchain->GetExtent();

	// Viewport state
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Rasterizer info
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	// Multisampling data
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// Colour blending data
	VkPipelineColorBlendAttachmentState colourBlendAttachment = {};
	colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colourBlendAttachment.blendEnable = VK_TRUE;
	colourBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colourBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colourBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colourBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	// Colour blend state
	VkPipelineColorBlendStateCreateInfo colourBlendState = {};
	colourBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colourBlendState.logicOpEnable = VK_FALSE;
	colourBlendState.logicOp = VK_LOGIC_OP_COPY;
	colourBlendState.attachmentCount = 1;
	colourBlendState.pAttachments = &colourBlendAttachment;
	colourBlendState.blendConstants[0] = 0.0f;
	colourBlendState.blendConstants[1] = 0.0f;
	colourBlendState.blendConstants[2] = 0.0f;
	colourBlendState.blendConstants[3] = 0.0f;

	// Dynamic state setup
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	// Pipeline layout creation info
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	
	// Create pipeline layout
	if (vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create pipeline layout!");
	}

	// Pipeline creation info
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(m_ShaderStages.size());
	pipelineInfo.pStages = m_ShaderStages.data();

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colourBlendState;
	pipelineInfo.pDynamicState = &dynamicState;
	
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = m_RenderPass->GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	// Create graphics pipeline
	if (vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Unable to create graphics pipeline!");
	}
}

// Destructor
GraphicsPipeline::~GraphicsPipeline(){
	// Destroy graphics pipeline
	vkDestroyPipeline(m_Device->GetDevice(), m_GraphicsPipeline, nullptr);

	// Destroy pipeline layout
	vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
}

// Bind graphics pipeline to command buffer
void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer){
	// Bind graphics pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}

// Add shader to pipeline shader stage
void GraphicsPipeline::AddShader(Shader* shader){
	// Pipeline shader stage creation info
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = shader->GetShaderStage();
	shaderStageCreateInfo.module = shader->GetShaderModule();
	shaderStageCreateInfo.pName = "main";

	// Add shader stage to stages
	m_ShaderStages.emplace_back(shaderStageCreateInfo);
}
