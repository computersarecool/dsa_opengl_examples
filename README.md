# OpenGL Examples

*Examples of modern OpenGL*

### Overview
These are a collection of examples of modern OpenGL (i.e. using DSA).

### Project layout
- Header files are in `include` (those specific to the exaples are in `include/projects`)
- `.cpp` files for the examples are in `src/projects`

#### Dependencies
- `cmake` (for building)

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

### License
:copyright: Willy Nolan 2017

[MIT License](LICENSE.txt)
