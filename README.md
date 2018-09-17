# DSA OpenGL Examples
*Examples of DSA OpenGL*

## Description
These are a collection of examples of modern OpenGL and Direct Sate Access (DSA).

## Tested On
- Linux
- Windows

## To Build
This project uses [Git LFS](https://git-lfs.github.com/) for assets and CMAKE to build
- Update all the submodules with: `git submodule update --init --recursive`
- `cd` into the `build` directory and type: `cmake -G "${GENERATOR_TYPE}"`
Note: The only `${GENERATOR_TYPE}` that has been tested is `Visual Studio 15 2017 Win64` as Clion reads the CMAKE file directly

## To Use
- Run the individual examples after building. 
Note: Paths to assets are relative and expect your current working directory to be where the binary is (i.e. `/bin/basic_cube/Debug`)

## Project Structure
- `bin`: This is where the final binary applications are put
- `build`: This holds the generates build files
- `include`: This includes headers from this project and third parties
- `src`: This holds the source code for these examples

## Functionality

#### basic_cube
This is a rotating cube with model-space positions as colors
  
#### multiple_attributes_and_buffers
This uses mapped buffers to upload data

The buffer backing the VAO is switched to change vertex data quickly

The attribute binding could also be switched to change the vertex data quickly

Interactivity: The spacebar toggles which buffer backs the active VAO 
  
#### compute_shader
This renders a cube to an FBO then uses a compute shader to invert that image
  
#### texture
This loads an image in the S3TC format

It checks to make sure it is compressed and gets the compressed size

#### texture_array
This loads a jpeg image and stores it in the S3TC compressed format

It checks that it is compressed and gets the compressed size

#### tesselation
This compares the various tesselation spacing options

Interactivity: The up arrow increases tesselation, down arrow decreases tesselationThis compares the var
  
#### tesselation_displacement_map
This uses a displacment map to offset vertices and tesselation

This uses an instanced quad with vertices embedded in the shader (there are no vertex attributes)

Interactivity: `w` key toggles showing wireframeThis uses a displacment map to offset vertices and tesselation
  
#### transform_feedback
This calculates the square root of some values and reads it back with transform feedback

There is no graphical output

#### geometry_shader
This is a pass through geometry shader
  
#### geometry_shader_normal_viewer
This is a cube geometry shader which converts faces to lines to show normals

#### framebuffers
This renders to a texture using and FBO
  
#### read_pixels
This uses glReadnPixels a safer version of glReadPixels and mapped Pixel Buffer Objects to improve performance

It saves the rendered image as a .tga file

Interactivity: Press spacebar to take a screenshot

#### point_sprites
This is a very simple point sprite example
  
#### phong_lighting
This is a Phong lighting example

Interactivity: This implements cursor lock to control camera position

Mouse wheel to move forward / black

#### raytracer
A raytracer

## Extra Notes
- To use these examples your graphics card must support at least OpenGL 4.5

### License

:copyright: Willy Nolan 2018

[MIT License](http://en.wikipedia.org/wiki/MIT_License)

