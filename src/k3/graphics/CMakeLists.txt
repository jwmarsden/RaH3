
set (CMAKE_CXX_STANDARD 17)

#  Include TinyObjectLoader
set(TINY_DIR ${PROJECT_SOURCE_DIR}/vendor/tinyobjloader)
include_directories(${TINY_DIR})

# Include Dear ImGui
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/vendor/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# Include glm
set(GLM_DIR ${PROJECT_SOURCE_DIR}/vendor/glm)
include_directories(${GLM_DIR})

# Include GLFW
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/vendor/glfw)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} CreateTriangle_glfw_binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Libraries
find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan;")

# Add Source
file(GLOB SOURCES *.cpp)

add_library(
    graphics 
    ${SOURCES} 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp 
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp 
    ${IMGUI_DIR}/imgui_widgets.cpp
)

target_link_libraries(
    graphics 
    ${LIBRARIES}
)

target_link_directories(
    graphics PUBLIC 
    ${GLFW_DIR}
    ${GLM_DIR}
)

target_include_directories(
    graphics PUBLIC 
    ${GLFW_DIR}
    ${GLM_DIR}
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)

target_compile_definitions(graphics PUBLIC -DImTextureID=ImU64)


# Copy Resources - Shaders
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.frag.spv ${CMAKE_BINARY_DIR}/shaders/simple_shader.frag.spv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.vert.spv ${CMAKE_BINARY_DIR}/shaders/simple_shader.vert.spv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.frag.spv ${CMAKE_BINARY_DIR}/Debug/shaders/simple_shader.frag.spv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.vert.spv ${CMAKE_BINARY_DIR}/Debug/shaders/simple_shader.vert.spv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.frag.spv ${CMAKE_BINARY_DIR}/Release/shaders/simple_shader.frag.spv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/k3/graphics/shaders/simple_shader.vert.spv ${CMAKE_BINARY_DIR}/Release/shaders/simple_shader.vert.spv COPYONLY)

#configure_file(${PROJECT_SOURCE_DIR}/models/teapot.obj ${CMAKE_BINARY_DIR}/models/teapot.obj COPYONLY)
#configure_file(${PROJECT_SOURCE_DIR}/models/teapot.obj ${CMAKE_BINARY_DIR}/Debug/models/teapot.obj COPYONLY)
#configure_file(${PROJECT_SOURCE_DIR}/models/teapot.obj ${CMAKE_BINARY_DIR}/Release/models/teapot.obj COPYONLY)