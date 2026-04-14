#pragma once

#include <SDL.h>
#include <vulkan/vulkan.h>
#include <vector>

namespace vk
{
    struct ContextCreateInfo
    {
        bool enableValidationLayers = true;
        VkDebugUtilsMessageSeverityFlagBitsEXT minLogSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    };

    class Context
    {
    public:
        void initialize(SDL_Window *window, const ContextCreateInfo &info = {});
        void shutdown();

        VkInstance getInstance() const { return instance; }
        VkSurfaceKHR getSurface() const { return surface; }

    private:
        SDL_Window *window = nullptr;
        VkInstance instance = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        bool enableValidationLayers = false;
        VkDebugUtilsMessageSeverityFlagBitsEXT minLogSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        static const std::vector<const char *> validationLayers;

        bool checkValidationLayerSupport() const;
        std::vector<const char *> getRequiredExtensions() const;

        static VkDebugUtilsMessageSeverityFlagsEXT makeSeverityMask(
            VkDebugUtilsMessageSeverityFlagBitsEXT minSeverity);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void setupDebugMessenger();
        void destroyDebugMessenger();
        bool isDeviceSuitable(VkPhysicalDevice device);
    };
}
