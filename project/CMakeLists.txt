cmake_minimum_required ( VERSION 4.0.0 )

project ( project )

# Find shaders used by OpenGL target.
file(GLOB_RECURSE GL_SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/glsl_shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/glsl_shaders/*.frag"
)
source_group("Shaders\\OpenGL" FILES ${GL_SHADERS})

add_executable(${PROJECT_NAME}
    main.cpp
    ${GL_SHADERS}
)

target_link_libraries ( ${PROJECT_NAME} labhelper )
config_build_output(${PROJECT_NAME})

# Vulkan executable (new path).
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

if("${SDL2_LIBRARIES}" STREQUAL "")
    message(STATUS "SDL2_LIBRARIES wasn't set, manually setting to SDL2::SDL2")
    set(SDL2_LIBRARIES "SDL2::SDL2")
endif()

file(GLOB_RECURSE VK_SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/vk_shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/vk_shaders/*.frag"
)

find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/Bin32"
)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Install the Vulkan SDK or add glslc to PATH.")
endif()

set(VK_SHADER_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/bin/vk_shaders")

set(VK_SPV_SHADERS "")
foreach(SHADER ${VK_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_OUTPUT "${VK_SHADER_RUNTIME_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VK_SHADER_RUNTIME_DIR}"
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
        VERBATIM
    )

    list(APPEND VK_SPV_SHADERS ${SPIRV_OUTPUT})
endforeach()

add_custom_target(vk_shaders_spv DEPENDS ${VK_SPV_SHADERS})

add_executable(project_vk
    vk_main.cpp
    vk_components/vk_context.h
    vk_components/vk_context.cpp
    vk_components/vk_renderer.h
    vk_components/vk_renderer.cpp
    vk_components/vk_swapchain.h
    vk_components/vk_swapchain.cpp
    vk_components/vk_pipeline.h
    vk_components/vk_pipeline.cpp
    vk_components/scene.h
    vk_components/scene.cpp
    vk_components/pipelines/background_pipeline.h
    vk_components/pipelines/background_pipeline.cpp
    vk_components/pipelines/simple_pipeline.h
    vk_components/pipelines/simple_pipeline.cpp
    vk_components/pipelines/shading_pipeline.h
    vk_components/pipelines/shading_pipeline.cpp
    vk_components/util.h
)
add_dependencies(project_vk vk_shaders_spv)

target_include_directories(project_vk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(project_vk PRIVATE ${SDL2_LIBRARIES} Vulkan::Vulkan labhelper)
config_build_output(project_vk)