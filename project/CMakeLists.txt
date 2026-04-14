cmake_minimum_required ( VERSION 4.0.0 )

project ( project )

# Find shaders used by OpenGL target.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)
source_group("Shaders" FILES ${SHADERS})

# OpenGL executable (existing path).
add_executable ( ${PROJECT_NAME}
    main.cpp
    ${SHADERS}
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

add_executable(project_vk
    vk_main.cpp
    vk_components/vk_context.h
    vk_components/vk_context.cpp
    vk_components/vk_renderer.h
    vk_components/vk_renderer.cpp
    vk_components/vk_swapchain.h
    vk_components/vk_swapchain.cpp
    vk_components/scene.h
    vk_components/scene.cpp
)

target_include_directories(project_vk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(project_vk PRIVATE ${SDL2_LIBRARIES} Vulkan::Vulkan labhelper)
config_build_output(project_vk)