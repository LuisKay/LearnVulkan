#pragma once

#include "001_vk_color_app.h"

class VKLineApp : public VKColorApp
{
    public:
        VKLineApp() {};
        ~VKLineApp() {};
        virtual void initVulkan() override;
        virtual void render() override;
        virtual void cleanup() override;
        virtual void cleanupSwapChain() override;
        virtual void reset(ANativeWindow *newWindow, AAssetManager *newManager) override;
    protected:
        virtual void fillVertexData() override;
        virtual void createGraphicsPipeline() override;
        virtual void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;
};
