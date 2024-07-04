#include "003_vk_line_app.h"

void VKLineApp::createMeshBuffers()
{
    // vertex data
    // right-handed system, with Y axis downward.
    // X axis right, Y axis downward, Z axis backward. 
    std::vector<Vertex> vertices = {
        {
            {-0.5, -0.5, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {0.5, -0.5, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {-0.5, 0.5, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {0.5, 0.5, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {-1.0, -1.0, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {1.0, -1.0, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {-1.0, 1.0, 0.0}, {0.67, 0.1, 0.2}
        },
        {
            {1.0, 1.0, 0.0}, {0.67, 0.1, 0.2}
        }
    };

    // indices data
    std::vector<uint16_t> indices = {0, 5, 1, 7, 2, 4, 3, 6};
    indicesCount = (uint32_t)indices.size();

    // temp vertex buffer
    VertexBuffer tempVertexBuffer;
    // temp index buffer
    IndexBuffer tempIndexBuffer;

    void *dataPtr = nullptr;
    VkMemoryRequirements memReqInfo;
    VkMemoryAllocateInfo memAllocInfo;
    ZeroVulkanStruct(memAllocInfo, VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);

    // create temp vertex buffer
    VkBufferCreateInfo vertexBufferInfo;
    ZeroVulkanStruct(vertexBufferInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    vertexBufferInfo.size = vertices.size() * sizeof(Vertex);
    vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VK_CHECK(vkCreateBuffer(device, &vertexBufferInfo, VULKAN_CPU_ALLOCATOR, &tempVertexBuffer.buffer));

    // get vertex buffer memory requirements
    vkGetBufferMemoryRequirements(device, tempVertexBuffer.buffer, &memReqInfo);

    // allocate temp vertex buffer memory
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    memAllocInfo.allocationSize = memReqInfo.size;
    memAllocInfo.memoryTypeIndex = findMemoryType(memReqInfo.memoryTypeBits, properties);
    VK_CHECK(vkAllocateMemory(device, &memAllocInfo, VULKAN_CPU_ALLOCATOR, &tempVertexBuffer.memory));
    // attach vertex memory to temp vertex buffer
    VK_CHECK(vkBindBufferMemory(device, tempVertexBuffer.buffer, tempVertexBuffer.memory, 0));
    // mapping dataPtr pointer (vertices) to temp vertex buffer memory
    VK_CHECK(vkMapMemory(device, tempVertexBuffer.memory, 0, memAllocInfo.allocationSize, 0, &dataPtr));
    std::memcpy(dataPtr, vertices.data(), vertexBufferInfo.size);
    vkUnmapMemory(device, tempVertexBuffer.memory);

    // local device vertex buffer
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VK_CHECK(vkCreateBuffer(device, &vertexBufferInfo, VULKAN_CPU_ALLOCATOR, &vertexBuffer.buffer));
    vkGetBufferMemoryRequirements(device, vertexBuffer.buffer, &memReqInfo);
    properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    memAllocInfo.allocationSize  = memReqInfo.size;
    memAllocInfo.memoryTypeIndex = findMemoryType(memReqInfo.memoryTypeBits, properties);
    VK_CHECK(vkAllocateMemory(device, &memAllocInfo, VULKAN_CPU_ALLOCATOR, &vertexBuffer.memory));
    // attach vertex memory to vertex buffer
    VK_CHECK(vkBindBufferMemory(device, vertexBuffer.buffer, vertexBuffer.memory, 0));

    // index buffer
    VkBufferCreateInfo indexBufferInfo;
    ZeroVulkanStruct(indexBufferInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    indexBufferInfo.size  = indicesCount * sizeof(uint16_t);
    indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VK_CHECK(vkCreateBuffer(device, &indexBufferInfo, VULKAN_CPU_ALLOCATOR, &tempIndexBuffer.buffer));

    // get index buffer memory requirements
    vkGetBufferMemoryRequirements(device, tempIndexBuffer.buffer, &memReqInfo);
    // allocate temp index buffer memory
    properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    memAllocInfo.allocationSize = memReqInfo.size;
    memAllocInfo.memoryTypeIndex = findMemoryType(memReqInfo.memoryTypeBits, properties);
    VK_CHECK(vkAllocateMemory(device, &memAllocInfo, VULKAN_CPU_ALLOCATOR, &tempIndexBuffer.memory));
    // attach index buffer memory to temp index buffer
    VK_CHECK(vkBindBufferMemory(device, tempIndexBuffer.buffer, tempIndexBuffer.memory, 0));
    // mapping dataPtr pointer (indices) to temp index buffer memory
    VK_CHECK(vkMapMemory(device, tempIndexBuffer.memory, 0, memAllocInfo.allocationSize, 0, &dataPtr));
    std::memcpy(dataPtr, indices.data(), indexBufferInfo.size);
    vkUnmapMemory(device, tempIndexBuffer.memory);

    // local device index buffer
    indexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VK_CHECK(vkCreateBuffer(device, &indexBufferInfo, VULKAN_CPU_ALLOCATOR, &indicesBuffer.buffer));
    vkGetBufferMemoryRequirements(device, indicesBuffer.buffer, &memReqInfo);
    properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    memAllocInfo.allocationSize  = memReqInfo.size;
    memAllocInfo.memoryTypeIndex = findMemoryType(memReqInfo.memoryTypeBits, properties);
    VK_CHECK(vkAllocateMemory(device, &memAllocInfo, VULKAN_CPU_ALLOCATOR, &indicesBuffer.memory));
    // attach index buffer memory to index buffer
    VK_CHECK(vkBindBufferMemory(device, indicesBuffer.buffer, indicesBuffer.memory, 0));
    
    VkCommandBuffer xferCmdBuffer;
    VkCommandBufferAllocateInfo xferCmdBufferInfo;
    ZeroVulkanStruct(xferCmdBufferInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
    // better to use transfer queue for data transfer op
    // here graphics queue is used for simplification.
    // it seems error is here, driver related.
    xferCmdBufferInfo.commandPool = commandPool;
    xferCmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    xferCmdBufferInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &xferCmdBufferInfo, &xferCmdBuffer));

    // Begin recording commands
    VkCommandBufferBeginInfo cmdBufferBeginInfo;
    ZeroVulkanStruct(cmdBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
    VK_CHECK(vkBeginCommandBuffer(xferCmdBuffer, &cmdBufferBeginInfo));

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertices.size() * sizeof(Vertex);
    vkCmdCopyBuffer(xferCmdBuffer, tempVertexBuffer.buffer, vertexBuffer.buffer, 1, &copyRegion);

    copyRegion.size = indices.size() * sizeof(uint16_t);
    vkCmdCopyBuffer(xferCmdBuffer, tempIndexBuffer.buffer, indicesBuffer.buffer, 1, &copyRegion);

    // End recording commands
    VK_CHECK(vkEndCommandBuffer(xferCmdBuffer));

    // submit commands, and wait...
    VkSubmitInfo submitInfo;
    ZeroVulkanStruct(submitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &xferCmdBuffer;

    VkFenceCreateInfo fenceInfo;
    ZeroVulkanStruct(fenceInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO);
    fenceInfo.flags = 0;

    VkFence fence = VK_NULL_HANDLE;
    VK_CHECK(vkCreateFence(device, &fenceInfo, VULKAN_CPU_ALLOCATOR, &fence));
    VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence));
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, MAX_int64));

    vkDestroyFence(device, fence, VULKAN_CPU_ALLOCATOR);
    vkFreeCommandBuffers(device, commandPool, 1, &xferCmdBuffer);

    vkDestroyBuffer(device, tempVertexBuffer.buffer, VULKAN_CPU_ALLOCATOR);
    vkFreeMemory(device, tempVertexBuffer.memory, VULKAN_CPU_ALLOCATOR);
    vkDestroyBuffer(device, tempIndexBuffer.buffer, VULKAN_CPU_ALLOCATOR);
    vkFreeMemory(device, tempIndexBuffer.memory, VULKAN_CPU_ALLOCATOR);
}

void VKLineApp::createGraphicsPipeline()
{
    auto vertShaderCode =
        LoadBinaryFileToVector("shaders/001_shader.vert.spv", assetManager);
    auto fragShaderCode =
        LoadBinaryFileToVector("shaders/001_shader.frag.spv", assetManager);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};

    // (triangle.vert):
    // layout (location = 0) in vec3 inPos;
    // layout (location = 1) in vec3 inColor;
    // Attribute location 0: Position
    // Attribute location 1: Color
    // vertex input bindding
    VkVertexInputBindingDescription vertexInputBinding = {};
    vertexInputBinding.binding   = 0; // Vertex Buffer 0
    vertexInputBinding.stride    = sizeof(Vertex); // Position + Color
    vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(2);
    // position
    vertexInputAttributs[0].binding  = 0;
    vertexInputAttributs[0].location = 0; // triangle.vert : layout (location = 0)
    vertexInputAttributs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[0].offset   = 0;
    // color
    vertexInputAttributs[1].binding  = 0;
    vertexInputAttributs[1].location = 1; // triangle.vert : layout (location = 1)
    vertexInputAttributs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributs[1].offset   = 12; // sizeof(float) * 3

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexInputBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // draw lines here
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // default line width
    rasterizer.lineWidth = 20.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                                    &pipelineLayout));
    // set VK_DYNAMIC_STATE_LINE_WIDTH here if you wish to set the line width 
    // for subsequent drawing commands when drawing using shader objects.
    // using vkCmdSetLineWidth(...)
    std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT,
                                                        VK_DYNAMIC_STATE_SCISSOR,
                                                        VK_DYNAMIC_STATE_LINE_WIDTH};
    VkPipelineDynamicStateCreateInfo dynamicStateCI{};
    dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
    dynamicStateCI.dynamicStateCount =
        static_cast<uint32_t>(dynamicStateEnables.size());

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicStateCI;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                        nullptr, &graphicsPipeline));
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void VKLineApp::initVulkan()
{
    createInstance();
    VKBaseApp::createSurface();
    pickPhysicalDevice();
    createLogicalDevicesAndQueue();
    VKBaseApp::setupDebugMessenger();
    establishDisplaySizeIdentity();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets(); 
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createMeshBuffers();
    createCommandBuffer();
    createSyncObjects();

    initialized = true;
    return;
}

void VKLineApp::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                  uint32_t imageIndex) 
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkDeviceSize offsets[1] = { 0 };

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkViewport viewport{};
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    static float black;
    black = 0.0f;
    VkClearValue clearColor = {{{black, black, black, 1.0f}}};

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                        VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetLineWidth(commandBuffer, 20.0f);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &descriptorSets[currentFrame],
                            0, nullptr);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indicesBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, indicesCount, 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    return;
}

void VKLineApp::render()
{
    if (orientationChanged) {
        onOrientationChange();
    }

    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    assert(result == VK_SUCCESS ||
            result == VK_SUBOPTIMAL_KHR);  // failed to acquire swap chain image
    updateUniformBuffer(currentFrame);

    vkResetFences(device, 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                            inFlightFences[currentFrame]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_SUBOPTIMAL_KHR) {
        orientationChanged = true;
    } else if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        VKColorApp::recreateSwapChain();
    } else {
        assert(result == VK_SUCCESS);  // failed to present swap chain image!
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return;
}

void VKLineApp::cleanup()
{
    VKColorApp::cleanup();
}

void VKLineApp::cleanupSwapChain()
{
    VKColorApp::cleanupSwapChain();
}

void VKLineApp::reset(ANativeWindow *newWindow, AAssetManager *newManager)
{
    VKColorApp::reset(newWindow, newManager);
}