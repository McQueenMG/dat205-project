
#include "vk_context.h"

#include <SDL_vulkan.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <map>

namespace vk
{
    const std::vector<const char *> Context::validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    void Context::initialize(SDL_Window *inWindow, const ContextCreateInfo &info)
    {
        window = inWindow;
        enableValidationLayers = info.enableValidationLayers;
        minLogSeverity = info.minLogSeverity;

        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested but not available.");
        }

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
    }

    void Context::shutdown()
    {
        if (surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(instance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }

        destroyDebugMessenger();

        if (instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }

        window = nullptr;
    }

    bool Context::checkValidationLayerSupport() const
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *requestedLayer : validationLayers)
        {
            bool found = false;
            for (const auto &available : availableLayers)
            {
                if (std::strcmp(requestedLayer, available.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<const char *> Context::getRequiredExtensions() const
    {
        unsigned int count = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr))
        {
            throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions failed: ") + SDL_GetError());
        }

        std::vector<const char *> extensions(count);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data()))
        {
            throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions(names) failed: ") + SDL_GetError());
        }

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    VkDebugUtilsMessageSeverityFlagsEXT Context::makeSeverityMask(
        VkDebugUtilsMessageSeverityFlagBitsEXT minSeverity)
    {
        switch (minSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        default:
            return VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Context::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *)
    {
        const char *sev = "UNKNOWN";
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            sev = "ERROR";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            sev = "WARN";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            sev = "INFO";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            sev = "VERBOSE";

        std::cerr << "[Vulkan " << sev << "] " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void Context::createInstance()
    {
        auto extensions = getRequiredExtensions();

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "project_vk";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "none";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("vkCreateInstance failed.");
        }
    }

    void Context::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device))
            {
                physicalDevice = device;
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                std::cout << "Selected GPU: " << deviceProperties.deviceName << std::endl;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool Context::isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;
    }

    void Context::createSurface()
    {
        if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
        {
            throw std::runtime_error(std::string("SDL_Vulkan_CreateSurface failed: ") + SDL_GetError());
        }
    }

    void Context::setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = makeSeverityMask(minLogSeverity);
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;

        auto createFn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

        if (createFn == nullptr)
        {
            throw std::runtime_error("Could not load vkCreateDebugUtilsMessengerEXT.");
        }

        if (createFn(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger.");
        }
    }

    void Context::destroyDebugMessenger()
    {
        if (debugMessenger == VK_NULL_HANDLE || instance == VK_NULL_HANDLE)
            return;

        auto destroyFn = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        if (destroyFn != nullptr)
        {
            destroyFn(instance, debugMessenger, nullptr);
        }

        debugMessenger = VK_NULL_HANDLE;
    }
}