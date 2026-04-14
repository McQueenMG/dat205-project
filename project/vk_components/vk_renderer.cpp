#include "vk_renderer.h"
#include "vk_context.h"
#include "scene.h"

namespace vk
{
    void Renderer::initialize(const Context &context)
    {
        swapchain.initialize(context);
        // TODO: Phase 2 - Command pools, pipelines, render passes
    }

    void Renderer::shutdown(const Context &context)
    {
        swapchain.shutdown(context);
        // TODO: Phase 2 - Cleanup pipelines, command pools
    }

    void Renderer::render(const Scene &scene)
    {
        // TODO: Phase 2 - Acquire, record, submit, present
    }
}
