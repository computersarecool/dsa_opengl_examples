# OpenGL Examples
*Examples of modern OpenGL*

## Description
These are a collection of examples of modern (DSA) OpenGL.

## Tested On
- Linux
- Windows

## To Build
This project uses CMAKE to build
- Update all the submodules with `git submodule update --init --recursive`
- `cd` into the `build` directory and type `cmake -G "${GENERATOR_TYPE}"` (The only `${GENERATOR_TYPE}` that has been tested is `Visual Studio 15 2017 Win64` as Clion reads the CMAKE file directly)

## To Use
- Run the individual examples after building

## Functionality

#### basic_cube

 A rotating cube with model-space positions as colors
  
#### multiple_attributes_and_buffers

  This uses mapped buffers to upload data
  The buffer backing the VAO is switched to change vertex data quickly
  The attribute binding could also be switched to change the vertex data quickly
  Interactivity: Spacebar toggles which buffer back the active VAO 
  
#### compute_shader

  This renders a cube to an FBO then uses a compute shader to invert that image
  
#### texture

  This loads an image in the S3TC format
  It checks to make sure it is compressed and gets the compressed size

#### texture_array
  
  This creates an array texture in the S3TC format

#### tesselation
  
  This compares the various tesselation spacing options
  Interactivity: Arrow up increased tesselation, arrow down decreases tesselation
  
#### tesselation_displacement_map
  
  An example that uses a displacment map to offset vertices and tesselation
  This uses an instanced quad with vertices embedded in the shader (there are no vertex attributes)
  
#### transform_feedback
  
  This calculates the square root of some values and reads it back with transform feedback
  No image is output

#### geometry_shader

  Pass through geometry shader
  
#### geometry_shader_normals
  
  A cube geometry shader which converts faces to lines to show normals

#### framebuffers
  
  Rendering to a texture example using FBOs
  
#### read_pixels
  
  This uses glReadnPixels a safer version of glReadPixels and mapped Pixel Buffer Objects to improve performance
  It saves the rendered image as a .tga file
  Interactivity: Press spacebar to take a screenshot

#### point_sprites
  
  A very simple point sprite example
  
#### lighting
  
  Phong lighting example
  Interactivity: This implements cursor lock to control camera position
  Mouse wheel to move forward / black

#### raytracer
  
  A raytracer

## Extra Notes
- Because this uses modern OpenGL your graphics card must support at least OpenGL 4.5

### License

:copyright: Willy Nolan 2018

[MIT License](http://en.wikipedia.org/wiki/MIT_License)

