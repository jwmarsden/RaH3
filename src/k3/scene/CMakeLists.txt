
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
include_directories(${GLFW_DIR}/include)

# Include ENTT
set(ENTT_DIR ${PROJECT_SOURCE_DIR}/vendor/entt)
include_directories(${ENTT_DIR}/single_include/entt)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Libraries
find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan;")

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/k3/scene/*.cpp)

add_library(scene ${sources})

target_link_libraries(
    scene 
    ${LIBRARIES}
)

