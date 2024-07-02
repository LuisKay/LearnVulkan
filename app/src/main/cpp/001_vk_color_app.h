#pragma once

#include "vk_app.h"

class VKColorApp : public VKBaseApp
{
    public:
        VKColorApp() {};
        ~VKColorApp() {};
        virtual void initVulkan() override;
        virtual void render() override;
        virtual void cleanup() override;
        virtual void cleanupSwapChain() override;
        virtual void reset(ANativeWindow *newWindow, AAssetManager *newManager) override;
    protected:
        virtual void createInstance() override;
        void pickPhysicalDevice();
        void createLogicalDevicesAndQueue();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDescriptorSetLayout();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffer();
        void createSyncObjects();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkValidationLayerSupport();

        VkShaderModule createShaderModule(const std::vector<uint8_t> &code);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void recreateSwapChain();
        void onOrientationChange();
        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties, VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);
        void createUniformBuffers();
        void updateUniformBuffer(uint32_t currentImage);
        void createDescriptorPool();
        void createDescriptorSets();
        void createMeshBuffers();
        void destroyMeshBuffers();
        void establishDisplaySizeIdentity();

        // physical device
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        // logical device
        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VertexBuffer vertexBuffer;
        IndexBuffer indicesBuffer;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkExtent2D displaySizeIdentity;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentFrame = 0;
        uint32_t indicesCount = 0;
        VkSurfaceTransformFlagBitsKHR pretransformFlag;
};
