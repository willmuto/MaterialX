# Prototype MaterialX Viewer

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/materialx/MaterialX/blob/master/LICENSE.txt)
[![Travis Build Status](https://travis-ci.org/materialx/MaterialX.svg?branch=master)](https://travis-ci.org/materialx/MaterialX)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/pmlxnp5m1fve11k0?svg=true)](https://ci.appveyor.com/project/jstone-lucasfilm/materialx)

This repository contains the **Prototype MaterialX Viewer** (MaterialXView), which leverages ShaderX to generate GLSL shaders from MaterialX graphs, rendering the results using the NanoGUI framework.  Both the standard set of MaterialX nodes and the PBR node set in ShaderX are supported.

### Example Images

**Standard Surface with procedural and uniform materials**
<img src="https://github.com/jstone-dev/MaterialX/blob/adsk_contrib/dev/documents/Images/MaterialXView_StandardSurface_01.png" width="512">

**Standard Surface with tiled texture materials**
<img src="https://github.com/jstone-dev/MaterialX/blob/adsk_contrib/dev/documents/Images/MaterialXView_StandardSurface_02.png" width="512">

### Quick Start for Developers

- Download the latest version of the [CMake](https://cmake.org/) build system.
- Point CMake to the root of the MaterialX library and generate C++ projects for your platform and compiler.
- Select the `MATERIALX_BUILD_VIEWER` option to build MaterialXView.

### Additional Resources

- The latest [ShaderX specification](https://github.com/jstone-dev/MaterialX/blob/adsk_contrib/dev/documents/Specification/ShaderX.Draft.pdf)
- The MaterialX graph for the [Standard Surface shader](https://github.com/jstone-dev/MaterialX/blob/adsk_contrib/dev/documents/Specification/ShaderX.Draft.pdf)
- An example MaterialX graph for [procedural marble](https://github.com/jstone-dev/MaterialX/blob/adsk_contrib/dev/documents/TestSuite/sxpbrlib/materials/standard_surface_marble_solid.mtlx)
