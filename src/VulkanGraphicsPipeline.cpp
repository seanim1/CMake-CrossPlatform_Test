#include "VulkanGraphicsPipeline.h"

/* from vulkan-tutorial.com */
std::vector<char> VulkanGraphicsPipeline::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char>& code, VkDevice logicalDevice) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}
void VulkanGraphicsPipeline::initSpecializationConstant()
{
	// Specialization Constant (Compile-time Constant used for compiler optimization) 
	// The constant_id can only be applied to a scalar *int*, a scalar *float* or a scalar * bool*
	// Map constant IDs to offsets in the buffer
	for (int i = 0; i < specialization_constant_count; i++) {
		specializationMapEntries[i].constantID = i; // 
		specializationMapEntries[i].offset = i * 4; // Offset in the specialization data buffer
		specializationMapEntries[i].size = sizeof(int); // Size of the data
	}
	// Define specialization info
	specializationInfo.mapEntryCount = specialization_constant_count;
	specializationInfo.pMapEntries = (VkSpecializationMapEntry*)specializationMapEntries; // Pointer to the map entry
	specializationInfo.dataSize = sizeof(gpuConstantData);	// Size of the specialization data
	specializationInfo.pData = &gpuConstantData;	// Pointer to the data

}
VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice logicalDevice,
	VkExtent2D swapChainExtent, VkSurfaceFormatKHR selectedSurfaceFormat,
	VkPipelineLayout uberPipelineLayout, Geometry* geometry)
{
	VulkanGraphicsPipeline::initSpecializationConstant();

	auto vertShaderCode = readFile("../shaders/box.vert.spv");
	auto fragShaderCode = readFile("../shaders/box.frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, logicalDevice);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, logicalDevice);

	VkPipelineShaderStageCreateInfo commonShaderStageInfo{};
	commonShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	commonShaderStageInfo.pSpecializationInfo = &specializationInfo;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = commonShaderStageInfo;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = commonShaderStageInfo;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// vertexInput - inputAssembly - Vertex Shader - Tessellation - Viewport - Raster - Multisample - DepthStencil (earlyFragTest) - Fragment Shader - ColorBlend (color attachment stage)

	/* Interleaved Vertex attribute setup: [Pos, Normal, Pos, Normal, Pos, Normal] */
	VkPipelineVertexInputStateCreateInfo vertexInputState = {};
	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(geometry->getBindingDescriptions().size());
	//vertexInputState.pVertexBindingDescriptions = geometry->getBindingDescriptions().data();
	//vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(geometry->getAttributeDescriptions().size());
	//vertexInputState.pVertexAttributeDescriptions = geometry->getAttributeDescriptions().data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// We either need this or define it in the pDynamicState, but one of them is required
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// (We don't need it but, required, otherwise validation error)
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// (We don't need it but, required, otherwise validation error)
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// Dynamic rendering attachment formats (there is no longer a need for VkRenderPass and VkFramebuffer)
	VkPipelineRenderingCreateInfo dynamicRendering{};
	dynamicRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	dynamicRendering.colorAttachmentCount = 1;
	dynamicRendering.pColorAttachmentFormats = &selectedSurfaceFormat.format;
	dynamicRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &dynamicRendering;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputState;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = uberPipelineLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}
