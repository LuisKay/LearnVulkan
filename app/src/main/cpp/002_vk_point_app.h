#pragma once

#include "001_vk_color_app.h"

class VKPointApp : public VKColorApp
{
    public:
        VKPointApp() {};
        ~VKPointApp() {};
        virtual void initVulkan() override;
        virtual void render() override;
        virtual void cleanup() override;
        virtual void cleanupSwapChain() override;
        virtual void reset(ANativeWindow *newWindow, AAssetManager *newManager) override;
    protected:
        virtual void createMeshBuffers() override;
        virtual void createGraphicsPipeline() override;
};
