#include <assert.h>
#include <array>

#include "utils.h"

std::vector<const char *> getRequiredExtensions(bool enableValidationLayers)
{
    std::vector<const char *> extensions;
    extensions.push_back("VK_KHR_surface");
    extensions.push_back("VK_KHR_android_surface");
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path,
                                            AAssetManager *assetManager) {
    std::vector<uint8_t> file_content;
    assert(assetManager);
    AAsset *file =
        AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
    size_t file_length = AAsset_getLength(file);

    file_content.resize(file_length);

    AAsset_read(file, file_content.data(), file_length);
    AAsset_close(file);
    return file_content;
}

const char *toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s) {
    switch (s) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        return "VERBOSE";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        return "ERROR";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        return "WARNING";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        return "INFO";
        default:
        return "UNKNOWN";
    }
}
const char *toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s) {
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT))
        return "General | Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) return "Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) return "General";
    return "Unknown";
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void * /* pUserData */) {
    auto ms = toStringMessageSeverity(messageSeverity);
    auto mt = toStringMessageType(messageType);
    printf("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

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
                          std::array<float, 16> &mat, float ratioX, float ratioY, float ratioZ) 
{
    // mat is initialized to the identity matrix
    // 
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    mat = {1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.};

    // scale is first
    // 
    // scaling factor is (ratioX, ratioY, ratioZ) --- (x, y, z)
    // 
    // | 1 0 0 0 |      | ratioX |
    // | 0 1 0 0 |  .   | ratioY |  =   
    // | 0 0 1 0 |      | ratioZ |
    // | 0 0 0 1 |      |   1    |
    // 
    // | ratioX   0     0    0 |
    // | 0     ratioY   0    0 |
    // | 0        0  ratioZ  0 |
    // | 0        0     0    1 |
    mat[0] *= ratioX;
    mat[5] *= ratioY;
    mat[10] *= ratioZ;

    // rotation is second
    // 
    // matrix to describe rotation (theta, short for t) along Z axis
    // 
    // | cos(t) -sin(t) 0 0 |
    // | sin(t) cos(t)  0 0 |
    // | 0      0       1 0 | 
    // | 0      0       0 1 |
    if (pretransformFlag & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
        // mat is set to a matrix to describe a 90 deg rotation along z axis, counterclockwise.
        // TODO, this is based on identity matrix, no scaling.
        mat = {0., 1., 0., 0., -1., 0, 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.};
    }

    else if (pretransformFlag & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        // mat is set to a matrix to describe a 270 deg rotation along z axis, counterclockwise.
        // TODO, this is based on identity matrix, no scaling.
        mat = {0., -1., 0., 0., 1., 0, 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.};
    }
}

void getGlmPrerotationMatrix(const VkSurfaceCapabilitiesKHR &capabilities,
                          const VkSurfaceTransformFlagBitsKHR &pretransformFlag,
                          glm::mat4 &mat, float ratioX, float ratioY, float ratioZ)
{
    // mat is initialized to the identity matrix.
    mat = glm::mat4(1.0f);

    // scale is first.
    mat = glm::scale(mat, glm::vec3(ratioX, ratioY, ratioZ));

    // rotation is second
    if (pretransformFlag & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
        // mat is set to a matrix to describe a 90 deg rotation along Z axis, counter clockwise.
        mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    else if (pretransformFlag & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        // mat is set to a matrix to describe a 270 deg rotation along Z axis, counter clockwise.
        mat = glm::rotate(mat, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    
    return;
}
