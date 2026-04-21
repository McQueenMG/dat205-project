#ifdef _WIN32
extern "C" __declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <stdexcept>

#include "vk_components/vk_context.h"
#include "vk_components/vk_renderer.h"
#include "vk_components/scene.h"

const uint32_t windowWidth = 1280;
const uint32_t windowHeight = 720;

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Vulkan Project",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    vk::Context context;
    vk::Renderer renderer;
    vk::Scene scene;

    try
    {
        vk::ContextCreateInfo ci{};
#ifdef NDEBUG
        ci.enableValidationLayers = false;
#else
        ci.enableValidationLayers = true;
#endif
        ci.minLogSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

        context.initialize(window, ci);
        renderer.initialize(context);
        scene.initialize();

        bool running = true;
        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = false;
                if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }

            renderer.render(scene);
            SDL_Delay(1);
        }

        scene.shutdown();
        renderer.shutdown(context);
        context.shutdown();

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}