<div align="center">

# ⚙️ IExeEngine ⚙️   
### *A Custom DirectX-Based Graphics Engine Built From Scratch*

</div>

---

## 🧩 Overview

This project represents my personal journey into **graphics programming and engine architecture**, developed entirely from scratch over the course of **three months** using **C++ and DirectX 11** inside **Visual Studio**.

Every component of this engine—from the compilation of C++ source files to rendering 3D geometry and managing camera transformations—was implemented manually to deeply understand how real-time rendering engines operate under the hood.

This README serves as an explanatory walkthrough of the topics, concepts, and systems that went into building my proprietary engine, following a natural chronological order:

1. **The C++ Build Process** – How code becomes an executable engine  
2. **Introduction to DirectX** – How DirectX and the GPU pipeline form the core of rendering  
3. **The Camera System** – How the engine interprets and transforms 3D space into what’s displayed on screen  

---

<div align="center">

# 🧱 C++ Build Process  
### *From Source Code to Executable Engine*

</div>

Before even reaching the graphics stage, it’s crucial to understand how C++ programs are built. This forms the technical foundation for everything the engine does.

### 🔹 Preprocessing
C++ begins as a collection of `.cpp` and `.h` files.  
The **preprocessor** scans these for directives like `#include`, `#define`, and macros.  
It effectively performs text substitution and header inclusion, producing what’s known as a **Translation Unit (TU)** — a single, self-contained file ready for compilation.

### 🔹 Compilation
The **compiler** translates each translation unit into **assembly code**, generating `.obj` files (object files).  
Here, syntax and semantic analysis occur. Any errors at this stage are **compile-time errors**, such as missing semicolons or mismatched types.

### 🔹 Linking
The **linker** then combines all object files and static libraries (`.lib`) into one unified **executable (`.exe`)**.  
This is where symbol resolution occurs — functions, classes, and variables are linked to their actual memory addresses.  
Errors like *“unresolved external symbol”* typically happen here.

Understanding this build pipeline allowed me to better structure my engine modularly, splitting it into core systems like:
- `Renderer.cpp`
- `Camera.cpp`
- `Mesh.cpp`
- `WinMain.cpp` (engine entry point)

---

<div align="center">

# 💻 Introduction to DirectX  
### *Bridging C++ and the GPU*

</div>

After setting up the C++ groundwork, the next step was implementing rendering through **DirectX 11**, the graphics API that powers this engine.

### 🔹 What is DirectX?
**DirectX** is a collection of low-level APIs designed for real-time multimedia and graphics applications.  
It communicates directly with the **GPU (Graphics Processing Unit)**, enabling high-performance rendering and real-time interactivity.

For this project, I specifically worked with **Direct3D 11**, the component responsible for 3D rendering.  
It’s the same API that powers games on both **Windows PCs** and **Xbox consoles**.

### 🔹 The Direct3D Device and Context
At the core of Direct3D are two key objects:
- **Device** – Manages creation and allocation of GPU resources (textures, buffers, shaders).
- **Device Context** – Issues rendering commands and controls the GPU pipeline.

Each rendered frame is drawn to a **Render Target View (RTV)** attached to the back buffer.  
When a frame is complete, the buffers **swap**, and the image appears on screen.

### 🔹 Resources and Views
DirectX uses **Resources** to store data such as:
- Geometry (vertex buffers)
- Textures
- Shader data

Each resource can be **bound** to the graphics pipeline through **Views** (e.g., Shader Resource Views or Render Target Views), defining how the data will be interpreted.

### 🔹 The Direct3D 11 Pipeline
The Direct3D 11 **Graphics Pipeline** processes data in several programmable and fixed stages:
1. **Input Assembler** – Collects vertex/index data.
2. **Vertex Shader** – Transforms 3D vertices to screen space.
3. **Geometry Shader** – (Optional) Creates or modifies primitives.
4. **Rasterizer** – Converts primitives to pixels.
5. **Pixel Shader** – Colors each pixel.
6. **Output Merger** – Combines color, depth, and stencil buffers for final output.

In my engine, I implemented the vertex and pixel shader stages first, providing the essential framework for rendering models on screen.

---

<div align="center">

# 🎥 The Camera System  
### *Transforming 3D Space to 2D Screen*

</div>

With rendering in place, the next milestone was implementing a **Camera System**, allowing the engine to simulate real-world perspective and navigation through 3D space.

### 🔹 The Transformation Pipeline
Rendering a 3D scene requires converting coordinates through several spaces:
```
Local Space → World Space → View Space → NDC Space → Screen Space
```
Each step applies a transformation using matrices.

### 🔹 Local Space
Each 3D model starts in **Local Space**, defined around its own origin or pivot.  
This allows for easy rotation, scaling, and translation relative to itself.

### 🔹 World Space
To place the object into the scene, it’s transformed into **World Space** using its **World Matrix**:
```
WorldMatrix = Scale * Rotation * Translation
```
This matrix determines the object’s position, orientation, and size in the global scene.

### 🔹 View Space (The Camera)
The **Camera** determines what part of the world is visible.  
It’s defined by two categories of properties:

- **External (Transform):** Position and rotation in the world.  
- **Internal (Lens):** Near plane, far plane, field of view (FOV), and aspect ratio.

The **View Matrix** moves the world so that the camera is positioned at the origin, facing down the Z-axis.  
It’s derived by inverting the camera’s world matrix:
```
ViewMatrix = inverse(WorldMatrixCamera)
```

This matrix ensures all scene geometry is viewed from the camera’s perspective.

### 🔹 Projection Matrix
The **Projection Matrix** converts the 3D world into a 2D perspective, simulating depth and field of view.

Two types of projection are possible:
- **Perspective Projection:** Used in FPS and 3D adventure games (depth-based scaling)
- **Orthographic Projection:** Used in puzzle or strategy games (uniform scaling)

In my engine, I used **Perspective Projection**, defined as:
```
ProjectionMatrix = PerspectiveFovLH(FOV, Aspect, NearZ, FarZ)
```
This maps 3D points to **Normalized Device Coordinates (NDC)**, ensuring all visible points fall between -1 and +1 in X/Y and 0–1 in Z.

### 🔹 Screen Transform
Finally, coordinates are mapped to **Screen Space**, scaled and translated to fit the display resolution:
```
x_screen = x_ndc * (width / 2) + (width / 2)
y_screen = -y_ndc * (height / 2) + (height / 2)
```

This is the final step before rasterization — converting the computed geometry into actual pixels.

---

<div align="center">

# 🧠 What I Learned

</div>

Building this engine from scratch revealed how much work modern engines like Unreal or Unity abstract away.  
Through this project, I gained hands-on experience with:

- The **complete C++ build process**, including linking and compilation  
- Initializing and managing **DirectX 11 rendering devices and resources**  
- Implementing **the graphics pipeline** manually using shaders  
- Understanding and building **camera transformations** using matrix math  
- Mapping **3D objects into screen space** via transformation pipelines  

This foundational understanding now allows me to confidently expand the engine further — adding lighting, materials, models, and potentially, real-time physics and post-processing.

---

<div align="center">

# 🧾 Conclusion

</div>

This project is more than just a graphics demo — it’s a fully functioning custom rendering engine that lays the groundwork for future real-time applications.  
Every subsystem, from the Direct3D initialization to camera projection, was built with clarity and performance in mind.  

Through this process, I developed a deep appreciation for how low-level graphics APIs like DirectX interact with both the CPU and GPU to bring pixels to life.

---

<div align="center">

# Animation For Games

https://github.com/user-attachments/assets/c8e6de55-bc71-4380-94d1-c02981ef62c0

That is a custom animation built in engine using models and animations from Mixamo, imported and converted into a suitable format for the engine and then played with a sound manager in order to create
an interesting scene from a very British movie.

[Animation Inspiration](https://www.youtube.com/watch?v=gHdDxKy2QW0)
<div align="center">

### 🖥️ GitHub Link
[🔗 View Repository on GitHub](https://github.com/IbrahimExe/IExeEngine)

</div>
