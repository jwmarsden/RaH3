cmake_minimum_required(VERSION 3.21)
project(kinetic C CXX)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
message("Setting Build Type Release")
option(KE_CONFIG_RELEASE "Release Mode On" ON)
endif ()

if(KE_CONFIG_RELEASE)
  add_definitions(-DKE_CONFIG_RELEASE)
endif()

# Include spdlog
include_directories(${PROJECT_SOURCE_DIR}/vendor/spdlog/include)

# Add headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add children 
add_subdirectory(${PROJECT_SOURCE_DIR}/src/k3/logging)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/k3/graphics)

# Add Source
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Create Executable
add_executable(kinetic ${sources})

target_include_directories(graphics PUBLIC GLM_DIR)

target_link_libraries(kinetic logging graphics)