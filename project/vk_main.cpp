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

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#include <imgui.h>

#include <perf.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <Model.h>
#include "hdr.h"
#include "fbo.h"

const uint32_t windowWidth = 1920;
const uint32_t windowHeight = 1024;

ivec2 g_prevMouseCoords = { -1, -1 };
bool g_isMouseDragging = false;

SDL_Window* g_window = nullptr;
float currentTime = 0.0f;
float previousTime = 0.0f;
float deltaTime = 0.0f;

bool handleEvents(vk::Scene &scene)
{
	// check events (keyboard among other)
	SDL_Event event;
	bool quitEvent = false;
	while(SDL_PollEvent(&event))
	{

		if(event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
		{
			quitEvent = true;
		}

		if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			g_isMouseDragging = true;
			int x;
			int y;
			SDL_GetMouseState(&x, &y);
			g_prevMouseCoords.x = x;
			g_prevMouseCoords.y = y;
		}

		if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			g_isMouseDragging = false;
		}

		if(event.type == SDL_MOUSEMOTION && g_isMouseDragging)
		{
			// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
			int delta_x = event.motion.x - g_prevMouseCoords.x;
			int delta_y = event.motion.y - g_prevMouseCoords.y;
			float rotationSpeed = 0.1f;
			mat4 yaw = rotate(rotationSpeed * deltaTime * -delta_x, scene.worldUp);
			mat4 pitch = rotate(rotationSpeed * deltaTime * -delta_y,
			                    normalize(cross(scene.cameraDirection, scene.worldUp)));
			scene.cameraDirection = vec3(pitch * yaw * vec4(scene.cameraDirection, 0.0f));
			g_prevMouseCoords.x = event.motion.x;
			g_prevMouseCoords.y = event.motion.y;
		}
	}

	// check keyboard state (which keys are still pressed)
	const uint8_t* state = SDL_GetKeyboardState(nullptr);
	vec3 cameraRight = cross(scene.cameraDirection, scene.worldUp);

	if(state[SDL_SCANCODE_W])
	{
        std::cout << "Current camera position: " << scene.cameraPosition.x << ", " << scene.cameraPosition.y << ", " << scene.cameraPosition.z << "\n"; 
		scene.cameraPosition += scene.cameraSpeed * deltaTime * scene.cameraDirection;
	}
	if(state[SDL_SCANCODE_S])
	{
		scene.cameraPosition -= scene.cameraSpeed * deltaTime * scene.cameraDirection;
	}
	if(state[SDL_SCANCODE_A])
	{
		scene.cameraPosition -= scene.cameraSpeed * deltaTime * cameraRight;
	}
	if(state[SDL_SCANCODE_D])
	{
		scene.cameraPosition += scene.cameraSpeed * deltaTime * cameraRight;
	}
	if(state[SDL_SCANCODE_Q])
	{
		scene.cameraPosition -= scene.cameraSpeed * deltaTime * scene.worldUp;
	}
	if(state[SDL_SCANCODE_E])
	{
		scene.cameraPosition += scene.cameraSpeed * deltaTime * scene.worldUp;
	}
	return quitEvent;
}


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

        bool stopRunning = false;
        float currentTime = 0.f;
        float previousTime = 0.f;
        float deltaTime = 0.f;
        bool stopRendering = false;
        int currentWidth = windowWidth;
        int currentHeight = windowHeight;
	    auto startTime = std::chrono::system_clock::now();

        while (!stopRunning)
        {
            std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
            previousTime = currentTime;
            currentTime = timeSinceStart.count();
            deltaTime = currentTime - previousTime;

            stopRunning = handleEvents(scene);
            SDL_GetWindowSize(window, &currentWidth, &currentHeight);
            scene.update(currentTime, currentWidth, currentHeight);
            renderer.render(scene);
            SDL_Delay(1);
        }

        scene.shutdown();
        renderer.shutdown(context.getDevice());
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

