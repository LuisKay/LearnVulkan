#pragma once

#include "utils.h"
#include <string>

class VKBaseApp
{
    public:
        VKBaseApp() {};
        ~VKBaseApp() {};

        virtual void initVulkan() {
            createInstance();
            createSurface();
            setupDebugMessenger();

            return;
        };
        virtual void render() = 0;
        virtual void cleanup() {
            destroyDebugMessenger();
            destroySurface();
            destroyInstance();

            return;
        }
        virtual void cleanupSwapChain() = 0;
        virtual void reset(ANativeWindow *newWindow, AAssetManager *newManager) {
            window.reset(newWindow);
            assetManager = newManager;
            if (initialized) {
                createSurface();
                // recreateSwapChain();
            }

            return;
        };
    protected:
        virtual void createInstance() { return; }

        virtual void destroyInstance() {
            if (instance) {
                vkDestroyInstance(instance, nullptr);
            }

            return;
        }

        virtual void setupDebugMessenger() {
            if (!enableValidationLayers) {
                return;
            }

            VkDebugUtilsMessengerCreateInfoEXT createInfo{};
            populateDebugMessengerCreateInfo(createInfo);

            VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                                    &debugMessenger));

            return;
        }

        virtual void destroyDebugMessenger() {
            if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            return;
        }

        /*
        * createSurface can only be called after the android ecosystem has had the
        * chance to provide a native window. This happens after the APP_CMD_START event
        * has had a chance to be called.
        *
        * Notice the window.get() call which is only valid after window has been set to
        * a non null value
        */
        virtual void createSurface() {
            assert(window != nullptr);  // window not initialized
            const VkAndroidSurfaceCreateInfoKHR create_info{
                .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .window = window.get()};

            VK_CHECK(vkCreateAndroidSurfaceKHR(instance, &create_info,
                                                nullptr /* pAllocator */, &surface));

            return;
        }

        virtual void destroySurface() {
            if (surface) {
                vkDestroySurfaceKHR(instance, surface, nullptr);
            }

            return;
        }

    public:
        bool initialized = false;

    protected:
        struct GPUBuffer {
            VkDeviceMemory memory;
            VkBuffer buffer;

            GPUBuffer()
                : memory(VK_NULL_HANDLE)
                , buffer(VK_NULL_HANDLE) {}
        };

        typedef GPUBuffer IndexBuffer;
        typedef GPUBuffer VertexBuffer;
        // typedef GPUBuffer UBOBuffer;

        /*
        * In order to enable validation layer toggle this to true and
        * follow the README.md instructions concerning the validation
        * layers. You will be required to add separate vulkan validation
        * '*.so' files in order to enable this.
        *
        * The validation layers are not shipped with the APK as they are sizeable.
        */
        bool enableValidationLayers = false;
        bool orientationChanged = false;

        VkInstance instance;
        VkSurfaceKHR surface;

        std::unique_ptr<ANativeWindow, ANativeWindowDeleter> window;
        AAssetManager *assetManager;

        VkDebugUtilsMessengerEXT debugMessenger;

        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

extern VKBaseApp* CreateVKApp();
