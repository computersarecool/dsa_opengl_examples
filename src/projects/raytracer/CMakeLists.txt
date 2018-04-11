# Set project name
set(PROJECT_NAME "raytracer")

# Set directory for the binary
set(PROJECT_BUILD_DIR ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIR}/$<CONFIG>)

# Set sources
set(PROJECTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/projects)
set(SOURCES
  src/main.cpp
  ${INCLUDE_DIR}/glad/src/glad.c
  ${INCLUDE_DIR}/glm/glm/gtc/matrix_transform.hpp
  ${INCLUDE_DIR}/projects/base_app.h
  ${INCLUDE_DIR}/projects/glsl_program.h
  ${INCLUDE_DIR}/projects/camera.h
  ${PROJECTS_SOURCE_DIR}/base_app/base_app.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/glsl_program.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/camera.cpp
  )

# Add executable to be built
add_executable(${PROJECT_NAME} ${SOURCES})

# Specify libraries to be used
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} glfw ${GLFW_LIBRARIES})

# Copy resources
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${PROJECT_BUILD_DIR}/assets)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Set correct CWD for debugger
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
endif()
