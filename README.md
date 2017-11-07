# OpenGL Examples

*Examples of modern OpenGL*

### Overview
These are a collection of examples that I am making while reading The OpenGL Superbible (7th Edition).  I am very excited to start working with Vulkan so these are minimum working examples.

#### Dependencies
- `glad`
- `glfw`
- `glm` >= 0.9.8.5
- `cmake`

This project uses CMAKE to build.

### Things to do
- Make GLFW a git submodule and see how that affects including glfw3.lib
- Fix `geometry shader quad`
- Make one media folder for all examples
- Make load image a function (a good example of this is the displacement map tesselation)
- Make setting up a texture a function (a good example of this is the displacement map tesselation)
- Remove duplicate code in `shader.cpp`
- Fix the wrong direction normals in the normal viewer example (winding order)
- Improve Bezier tesselation example
- Add descriptions to all examples

### Examples to add
- Anisotropic shading
- Uniform blocks
- Shader storage blocks
- Atomic counters
- Image load / store
- Shader subroutines
- Seperable shaders
- Mapped buffer
- GLSL Advanced functions (number packing)
- Shader binaries
- Instancing
- Indirect drawing
- Primitive restart
- User defined clipping
- Different streams in geometry shaders
- Adjacent primitive types
- Viewport arrays
- Scissor test

### Concepts to review
- Calling `glEnable` on: `GL_DEPTH_TEST`, `GL_CULL_FACE`, `GL_BLEND`
- `glBlendFunc` values
- Camera and projection matrices
- Samplers, texture units and textures (a good example of this is the displacement map tesselation)
- glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

### License
:copyright: Willy Nolan 2017

[MIT License](https://en.wikipedia.org/wiki/MIT_License)
