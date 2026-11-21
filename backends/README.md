# Backends

- [Graphics](#graphics)
  - [OpenGL2](#opengl2-legacy)
  - [OpenGL3](#opengl3-modern-opengl)
  - [DirectX9](#directx9-could-potentially-cover-more-then-opengl2)
  - [Citro3D](#citro3d-3ds)
  - [Metal](#metal-macos--ios)

## Graphics

### OpenGL2 (Legacy)

> [!NOTE]
> As SPIRV to GLSL 120 translation doesn't work, your'e forced to write separate shaders for this backend

### OpenGL3+ (Modern OpenGL)

### DirectX9 (Could potentially cover more than OpenGL2)

### Citro3D (3DS)

> [!NOTE]
> This backend requires shaders as binary file, directly in the code or as string for libpicasso
> The Fragment Stage is special and is still wip

### Metal (macOS / iOS)

> [!NOTE]
> The OS layer owns the frame (drawable, command buffer, render encoder) and
> shares it with the driver through a `PD::GfxMetalCtx`:
> `PD::Gfx::UseDriver<PD::GfxMetal>(&ctx);` and update `ctx.CmdBuf` /
> `ctx.Encoder` every frame (see `tests/gfx/source/os/metal-layer.cpp`).
> Shaders are the same SPIR-V blobs as OpenGL3, translated to MSL at runtime.

