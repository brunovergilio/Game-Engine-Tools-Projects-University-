# Game Engine / Tools Projects
Projects I worked on while pursuing my Master's Degree in Game Programming at DePaul University

# Engine Programming
## BDVEngineDX
Engine created using DirectX 11. Contains demos for:
```
* Animation and blending, with the interpolation done on both CPU or GPU
* Normal mapping
* Animation with compressed objects
```
Video for the projects -> https://www.youtube.com/watch?v=6FCanyHmRN4

# Rendering
## Research Work
Coded a deferred renderer using DirectX 11, along with support for point and spot light shadows, along with a few demos:
```
* BasicDeferredRendering - Simple deferred rendering demo
* DSSpotLightTest - Basic spot light testing within a deferred renderer
* InstancedPointLights - Multiple sphere and multiple point light sources, done with a single (instanced) draw call each (one for the spheres and another one for the light sources)
* PointLightShadows - Single-pass, cubemapped shadows for a point light source
* SpotLightShadows - Spot light shadow
* DirectionalLightShadows - (Failed) attempt at doing cascaded shadow mapping based on the HLSL Rendering Cookbook's CSM strategy
```
Video for the projects -> http://youtu.be/0L2LFK9FmRo?hd=1

# Tools Programming
## FBXExporter
Command-line C++ Implementation of an exporter for the FBX format created by Autodesk. It was used to acquire only the relevant data from the FBX files, since it contained a lot of data that we weren't using at the time. Features:
```
* Extracts vertex and index data from the fbx file
* Extracts texture
* Convert to / from LH / RH
* Compress the animation based on rotation angle (threshold)
* Process joints / keyframe data (if model is animated)
```

## NormalMapGUI
C# Implementation of a Procedural Normal Map Generator. It first converts the image to grayscale and then uses edge-detection algorithms to convert the image to a map of normal vectors, encoded in RGB colors (the Alpha component is optional for a depth map). Features:
```
* Different convolution kernels for edge-processing
* Border wrapping / clipping
* Axis inversion (X / Y)
* Bumpiness / Strength threshold for increased displacement of the normal vectors
* Option to include the depth map on the alpha component
```

## SpriteFont
Command-line C++ implementation of a Sprint Font generator. It uses Win32's GDI methods to Create a device context, write a set of glyphs with a given font to it, and then saves it to a png file with the FreeImage library. Features:
```
* Specify any font present in the system (name must match)
* Specify the size
* Specify qualities like bold, italic and aliased
* Set first and last characters (Utf-16)
* Set the number of columns
```
