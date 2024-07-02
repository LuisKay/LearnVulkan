#pragma once

#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

const int MAX_FRAMES_IN_FLIGHT = 2;

struct ANativeWindowDeleter {
    void operator()(ANativeWindow *window) { ANativeWindow_release(window); }
};

struct UniformBufferObject {
    glm::mat4 mvp;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

// struct UBOData {
//     Matrix4x4 model;
//     Matrix4x4 view;
//     Matrix4x4 projection;
// };

struct Vertex {
    float position[3];
    float color[3];
};

#define LOG_TAG "hellovk"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define VK_CHECK(x)                           \
    do {                                        \
        VkResult err = x;                         \
        if (err) {                                \
            LOGE("Detected Vulkan error: %d", err); \
            abort();                                \
        }                                         \
    } while (0)

#define MAX_int64		((int64_t)	0x7fffffffffffffff)
#define VULKAN_CPU_ALLOCATOR nullptr

std::vector<const char *> getRequiredExtensions(bool enableValidationLayers);

std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path,
                                            AAssetManager *assetManager);

const char *toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s);
const char *toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s);

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void * /* pUserData */);

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator);

/*
 * getPrerotationMatrix handles screen rotation with 3 hardcoded rotation
 * matrices (detailed below). We skip the 180 degrees rotation.
 * 
 * column-major order.
 * 
 * this is mainly for understanding, use glm is more convenient.
 * see getGlmPrerotationMatrix
 */
void getPrerotationMatrix(const VkSurfaceCapabilitiesKHR &capabilities,
                          const VkSurfaceTransformFlagBitsKHR &pretransformFlag,
                          std::array<float, 16> &mat, float ratioX, float ratioY, float ratioZ);

void getGlmPrerotationMatrix(const VkSurfaceCapabilitiesKHR &capabilities,
                          const VkSurfaceTransformFlagBitsKHR &pretransformFlag,
                          glm::mat4 &mat, float ratioX, float ratioY, float ratioZ);
template<class T>
inline void ZeroVulkanStruct(T& vkStruct, VkStructureType vkType)
{
	vkStruct.sType = vkType;
	memset(((uint8_t*)&vkStruct) + sizeof(VkStructureType), 0, sizeof(T) - sizeof(VkStructureType));
}
