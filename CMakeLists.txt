# Project initialization
cmake_minimum_required(VERSION 2.7.2 FATAL_ERROR)

project("opengl_examples")

# Compiler settings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # GNU compiler settings
  # Wall: Enable all warnings about constructors
  # Wextra: Enable some extra warnings
  # Werror: Treat all warnings as errors
  #  set(warnings "-Wall -Wextra -Werror")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Visual C++ (Microsoft)
  # W4: Show the most warnings
  # WX: Treat all warnings as errors
  # EHsc: Only emit code for exception filters when detected that the code in the try {} block might throw a C++ exception
#  set(warnings "/W4 /WX /EHsc")
#  set(CMAKE_CONFIGURATION_TYPES Debug Release)
endif()

# Set warnings
if (NOT CONFIGURED_ONCE)
  set(CMAKE_CXX_FLAGS "${warnings}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# Set variables used within this file
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECTS_DIR ${CMAKE_SOURCE_DIR}/src/projects)

# Build and include GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw)
include_directories(${LIB_DIR}/lib/glfw/include)

# Add include directories
include_directories(${INCLUDE_DIR} ${INCLUDE_DIR}/projects)

# Add subdirectories (projects)
add_subdirectory(${PROJECTS_DIR}/basic_cube)
#add_subdirectory(${PROJECTS_DIR}/geometry_shader)
#add_subdirectory(${PROJECTS_DIR}/geometry_shader_normals)
#add_subdirectory(${PROJECTS_DIR}/geometry_shader_quad)
#add_subdirectory(${PROJECTS_DIR}/lighting)
#add_subdirectory(${PROJECTS_DIR}/multiple_attributes_and_buffers)
#add_subdirectory(${PROJECTS_DIR}/tesselation)
#add_subdirectory(${PROJECTS_DIR}/tesselation_bezier)
#add_subdirectory(${PROJECTS_DIR}/tesselation_displacement_map)
#add_subdirectory(${PROJECTS_DIR}/texture2Darray)
#add_subdirectory(${PROJECTS_DIR}/transform_feedback)

# Create a variable that is set to true and cached after the first configuration
set(CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")
