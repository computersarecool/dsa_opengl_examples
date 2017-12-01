# OpenGL Examples

*Examples of modern OpenGL*

### Overview
These are a collection of examples of modern OpenGL (i.e. using DSA). Many are minimum working examples.

### Project layout
- `.cpp` files for the examples are in `src/projects`
- Header files are in `include` (those specific for the exaples are in) `include/projects`

#### Dependencies
- `cmake` (to build)

##### Included as submodules:
- `glfw`
- `glm`

##### Included as a snapshot:
- `glad` (a loader for OpenGL `4.4`)

Thanks to the package authors for their work! 👏

### To build and use:
This project uses CMAKE to build.

- Update all the submodules with `git submodule update --init --recursive`.
- `cd` into the `build` directory and type `cmake -G "${GENERATOR_TYPE}"`.

The only `${GENERATOR_TYPE}` that has been tested is `Visual Studio 15 2017 Win64` as KDevelop reads the CMAKE file directly and no generator is needed.

### Things to do
- Make a media folder for examples
- Make load image a function (a good example of where this is needed is the displacement map tesselation project)
- Make setting up a texture a function (a good example of this is the displacement map tesselation project)
- Remove duplicate code in `shader.cpp`
- Fix the wrong direction normals in the normal viewer example (winding order)
- Add descriptions to all examples

### Examples to add
- Anisotropic shading
- Uniform blocks
- Shader storage blocks
- Atomic counters
- Texture views
- Texture compression
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

[MIT License](LICENSE.txt)
