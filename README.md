<img width="1400" height="583" alt="image" src="https://github.com/user-attachments/assets/6372e8ea-9388-47ad-98f7-2415649f54c2" />


# Sherbert Vulkan Template

<!-- Header Start -->
<a href="https://isocpp.org/"> <img height="40" img width="40" src="https://cdn.simpleicons.org/cplusplus"></a>
<a href="https://cmake.org/"> <img height="40" img width="40" src="https://cdn.simpleicons.org/cmake"></a>
<a href="https://www.vulkan.org/"> <img height="40" img width="40" src="https://cdn.simpleicons.org/vulkan"></a>
<img align="right" alt="Stars Badge" src="https://img.shields.io/github/stars/jdsherbert/Sherbert-Vulkan-Template?label=%E2%AD%90"/>
<img align="right" alt="Forks Badge" src="https://img.shields.io/github/forks/jdsherbert/Sherbert-Vulkan-Template?label=%F0%9F%8D%B4"/>
<img align="right" alt="Watchers Badge" src="https://img.shields.io/github/watchers/jdsherbert/Sherbert-Vulkan-Template?label=%F0%9F%91%81%EF%B8%8F"/>
<img align="right" alt="Issues Badge" src="https://img.shields.io/github/issues/jdsherbert/Sherbert-Vulkan-Template?label=%E2%9A%A0%EF%B8%8F"/>
<!-- Header End --> 

-----------------------------------------------------------------------

<a href="https://www.vulkan.org/">
  <img align="left"
       alt="Vulkan Template"
       src="https://img.shields.io/badge/Vulkan%20Template-Full%20Project?style=for-the-badge&logo=vulkan&logoColor=white&color=A41E22&labelColor=A41E22">
</a>
  
<a href="https://choosealicense.com/licenses/mit/"> 
  <img align="right" alt="License" src="https://img.shields.io/badge/License%20:%20MIT-black?style=for-the-badge&logo=mit&logoColor=white&color=black&labelColor=black"> </a>
  
<br></br>

-----------------------------------------------------------------------
## Overview

Sherbert Vulkan Template is a lightweight Vulkan renderer starter project designed to get developers rendering as quickly as possible.

Rather than spending hours implementing Vulkan boilerplate before seeing anything on screen, this template provides a complete working renderer with the core setup already implemented. The project creates a Vulkan instance, selects a graphics device, creates a swapchain, configures a render pass and graphics pipeline, records command buffers, and presents a rendered triangle.

The goal of this repository is to provide a clean and understandable foundation for learning Vulkan or starting a custom rendering engine. The code is intentionally kept simple and exposed, allowing developers to follow the flow of Vulkan initialization without navigating through unnecessary engine abstractions.

Unlike many Vulkan tutorials, this template includes complete vertex and fragment shaders (`triangle.vert` and `triangle.frag`) alongside the renderer code. This means the entire path from application startup to GPU execution is included, making it easier to understand how Vulkan pipelines, shader stages, and rendering commands work together.


## Learning Resources

This repository is intended to be a practical starting point for learning Vulkan.

Included examples:
- Vulkan instance creation
- Physical and logical device setup
- Swapchain management
- Render pass creation
- Graphics pipeline setup
- Vertex and fragment shader stages
- Command buffer recording
- Frame presentation

The included triangle renderer provides a complete minimal example of the Vulkan rendering pipeline from CPU-side setup through to GPU execution.

## Features

- Vulkan 1.1 renderer setup
- Win32 window surface creation
- Physical & logical device selection
- Graphics queue initialization
- Swapchain creation
- Swapchain image views
- Render pass creation
- Graphics pipeline creation
- Command buffer recording
- Triangle rendering
- Presentation to the swapchain
- Modern C++ project structure
- CMake build system

- -----------------------------------------------------------------------

## Getting Started

### Requirements

- C++20 compatible compiler
- CMake 3.20+
- Vulkan SDK
- Git

### Building

```bash
git clone https://github.com/JDSherbert/Sherbert-Vulkan-Template.git
cd Sherbert-Vulkan-Template

cmake -B Build
cmake --build Build --config Release
```

