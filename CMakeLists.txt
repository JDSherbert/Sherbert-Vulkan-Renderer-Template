cmake_minimum_required(VERSION 4.0)
project(SherbertVulkanTemplate)

set(CMAKE_CXX_STANDARD 20)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Vulkan shaders are not compiled at runtime.
# They must be converted from GLSL source into SPIR-V bytecode before being loaded by Vulkan.
find_program(GLSLANG_VALIDATOR glslangValidator)

if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! Make sure Vulkan SDK Bin is in your PATH")
endif()

set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_BIN_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_BIN_DIR})

set(SHADERS
        triangle.vert
        triangle.frag
)

set(SPIRV_SHADERS "")

foreach(SHADER ${SHADERS})
    get_filename_component(NAME_WE ${SHADER} NAME_WE)
    set(SPIRV_FILE ${SHADER_BIN_DIR}/${SHADER}.spv)
    add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER_SRC_DIR}/${SHADER} -o ${SPIRV_FILE}
            DEPENDS ${SHADER_SRC_DIR}/${SHADER}
            COMMENT "Compiling ${SHADER} to SPIR-V"
            VERBATIM
    )
    list(APPEND SPIRV_SHADERS ${SPIRV_FILE})
endforeach()

# Create a custom target that builds all SPIR-V shaders
add_custom_target(Shaders ALL DEPENDS ${SPIRV_SHADERS})

# Add executable
add_executable(SherbertVulkanTemplate
        Source/Main.cpp
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Application/Window.cpp
        Source/Application/Window.h
        Source/Renderer/GraphicsDevice.cpp
        Source/Renderer/GraphicsDevice.h
        Source/Renderer/Pipeline.cpp
        Source/Renderer/Pipeline.h
        Source/Renderer/RenderPass.cpp
        Source/Renderer/RenderPass.h
        Source/Renderer/CommandBuffers.cpp
        Source/Renderer/CommandBuffers.h
)

# Make sure shaders are built first
add_dependencies(SherbertVulkanTemplate Shaders)

# Include project source directories
target_include_directories(SherbertVulkanTemplate PRIVATE Source)

# Link Vulkan
target_link_libraries(SherbertVulkanTemplate PRIVATE Vulkan::Vulkan)

# On Windows, also link user32 and gdi32 for Win32 window creation
if(WIN32)
    target_link_libraries(SherbertVulkanTemplate PRIVATE user32 gdi32)
endif()
