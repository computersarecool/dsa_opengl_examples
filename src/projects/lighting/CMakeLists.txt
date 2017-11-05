# Name for this project
set(PROJECT_NAME "lighting_example")

# Choose the library for the final build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set sources
set(PROJECTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/projects)
set(SOURCES
  main.cpp
  ${INCLUDE_DIR}/glad/glad.c
  ${INCLUDE_DIR}/glm/gtc/matrix_transform.hpp
  ${INCLUDE_DIR}/projects/base_app.h
  ${INCLUDE_DIR}/projects/shader.h
  ${INCLUDE_DIR}/projects/camera.h
  ${PROJECTS_SOURCE_DIR}/base_app/base_app.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/shader.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/camera.cpp
  )

# Use the directory name for the executable
add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
  target_link_libraries(${PROJECT_NAME} ${LIB_DIR}/glfw3.lib)
endif()
