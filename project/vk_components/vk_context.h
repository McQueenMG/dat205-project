#pragma once

#include <SDL.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace vk
{
    struct ContextCreateInfo
    {
        bool enableValidationLayers = true;
        VkDebugUtilsMessageSeverityFlagBitsEXT minLogSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    };
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };


    class Context
    {
    public:
        void initialize(SDL_Window *window, const ContextCreateInfo &info = {});
        void shutdown();

        VkInstance getInstance() const { return instance; }
        VkSurfaceKHR getSurface() const { return surface; }
        VkDevice getDevice() const { return device; }
        VkQueue getGraphicsQueue() const { return graphicsQueue; }
        VkQueue getPresentQueue() const { return presentQueue; }

    private:
        SDL_Window *window = nullptr;
        VkInstance instance = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

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
        void createLogicalDevice();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    };
}
