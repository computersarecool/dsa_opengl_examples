# Name for this project
set(PROJECT_NAME "read_pixels")

# Choose the library for the final build
set(PROJECT_BUILD_DIR ${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIR}/$<CONFIG>)

# Set sources
set(PROJECTS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/projects)
set(SOURCES
  src/main.cpp
  ${INCLUDE_DIR}/glad/src/glad.c
  ${INCLUDE_DIR}/glm/glm/gtc/matrix_transform.hpp
  ${INCLUDE_DIR}/projects/base_app.h
  ${INCLUDE_DIR}/projects/shader.h
  ${INCLUDE_DIR}/projects/camera.h
  ${PROJECTS_SOURCE_DIR}/base_app/base_app.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/shader.cpp
  ${PROJECTS_SOURCE_DIR}/base_app/camera.cpp
  )


add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} glfw ${GLFW_LIBRARIES})

# Copy resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BUILD_DIR}/)

# Set correct CWD for debugger
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BUILD_DIR}/Debug)
endif()
