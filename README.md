# LittleEngine

A lightweight, modular C++ game engine focused on 2D rendering, input, audio, and a simple UI layer — with interchangeable platform backends (GLFW or SDL) and a modern OpenGL renderer.

> **Targets:** C++17, CMake ≥ 3.16.  
> **Backends:** GLFW **or** SDL3 (select at configure time).  
> **Bundled deps:** GLAD, GLM, stb_image, FreeType, ImGui, miniaudio, (ENet vendored, currently optional).

---

## ✨ Features

- **Renderer (OpenGL + GLAD):** batched quads, textures, texture atlases, tilemap renderer, colors, simple lighting, render targets/FBOs, wireframe mode.
- **Camera:** 2D camera with transforms and view/projection helpers.
- **Text & Fonts:** FreeType integration; default font included.
- **Images:** `stb_image` for PNG/JPG loading.
- **Audio:** `miniaudio` for playback (WAV/MP3/etc. via miniaudio).
- **Input:** Keyboard & mouse events, callbacks.
- **Windowing:** Pluggable **GLFW** or **SDL3** platform layer, vsync, resizable or fixed windowed modes.
- **UI:** Dear **ImGui** integrated (toggle via compile definition).
- **Utils:** Logger, file system helpers, random, geometry, debug tools.
- **Structure:** Headers in `include/LittleEngine/**`, sources in `src/**`, backends in `src/Platform/{GLFW,SDL}`. Third-party code is vendored in `thirdparty/`.

---

## 🗂️ Repository Layout (high level)

```
include/LittleEngine/
  Audio/            // audio.h, sound.h
  Core/             // window.h (callbacks & config)
  Graphics/         // renderer.h, texture.h, shader.h, font.h, camera.h, render_target.h, ... 
  Input/            // input.h
  Math/             // geometry.h, random.h
  Platform/         // platform.h (USE_GLFW / USE_SDL switch)
  UI/               // ui_context.h, ui_element.h, ui_system.h
  Utils/            // logger.h, file_system.h, debug_tools.h
  internal.h
  little_engine.h   // public API (Initialize/Run/Shutdown, etc.)

src/
  Graphics/, Audio/, Input/, Math/, UI/, Utils/
  Platform/
    GLFW/           // glfw_platform.cpp, glfw_window.cpp, ...
    SDL/            // sdl_platform.cpp, sdl_window.cpp, ...
  little_engine.cpp // main engine loop & initialization

thirdparty/
  glad, glm, stb_image, freetype-2.13.3, imgui, miniaudio
  glfw-3.3.2, SDL/, enet-1.3.17 (vendored; optional)
```

---

## 🔧 Build

This project builds a **static library**: `LittleEngine`.

### Prerequisites
- C++17 compiler (MSVC, Clang, or GCC)
- CMake **3.16+**
- Python is **not** required; all third-party code is vendored and built via CMake.

### Configure options
- **`-DPLATFORM=GLFW`** *(default assumed)* or **`-DPLATFORM=SDL`** – selects the windowing/input backend.
- **`-DENABLE_IMGUI=1|0`** – ImGui integration toggle (the code paths use `ENABLE_IMGUI`; define at configure time if you want to disable).
- On Windows, **SIMD** may be enabled via `LittleEngine_SIMD` macro (defaults to 1 on `_WIN32`, 0 otherwise; see `include/LittleEngine/little_engine.h`). You can override if needed.

> The CMake script vendored here adds the necessary third-party subdirectories and compile definitions like `USE_GLFW` or `USE_SDL` based on `PLATFORM`.

### Build (generic)
```bash
cmake -S . -B build -DPLATFORM=GLFW -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Build (Windows / MSVC)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DPLATFORM=GLFW
cmake --build build --config Release -- /m
```

### Build (Linux/macOS)
```bash
cmake -S . -B build -DPLATFORM=SDL -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
```

The resulting static library target is named **`LittleEngine`**.

---

## 🚀 Using LittleEngine in your game

Link your game executable with the `LittleEngine` target (or install it and link regularly). A minimal usage pattern looks like:

```cpp
#include <LittleEngine/little_engine.h>

int main() {
    // Optional: customize engine config here (window, vsync, etc.)
    if (LittleEngine::Initialize(/* EngineConfig{} if provided */) != 0) return 1;

    // Fixed-step update (60Hz) + render callback provided to Run:
    LittleEngine::Run(
        [](float dt) {
            // update game state, input handling, etc.
        },
        []() {
            // begin frame, draw calls (textures, quads, text), end frame
        }
    );

    LittleEngine::Shutdown();
    return 0;
}
```

### Key headers you’ll typically use
- `LittleEngine/Graphics/renderer.h` – quads, textures, tilemaps, text, render targets.
- `LittleEngine/Graphics/texture.h`, `shader.h`, `font.h`
- `LittleEngine/Graphics/camera.h`
- `LittleEngine/Core/window.h` – window configuration & callbacks.
- `LittleEngine/Input/input.h` – key and mouse enums/events.
- `LittleEngine/Utils/logger.h`, `file_system.h`

> See `Platform/platform.h` to understand how `USE_GLFW`/`USE_SDL` select the implementation. ImGui hooks live behind `ENABLE_IMGUI` checks.

---

## 🖼️ Screenshots & Diagrams (recommended)

Adding images will greatly improve the README:
- **Hero screenshot** of a sample scene (sprites + text).
- **Tilemap renderer** showcase.
- **Text rendering** (antialiased, different sizes).
- **UI overlay** (ImGui demo or your in-engine UI).
- **Architecture diagram**: a simple block diagram of Modules → Renderer → Platform.

Place assets in `docs/` and reference them in this README, e.g.:
```md
![Demo](docs/demo.png)
![Architecture](docs/architecture.png)
```

If you’d like, I can sketch a simple architecture diagram (SVG) and placeholder screenshots/GIF frames.

---

## 🧪 Development Tips

- To switch backends, reconfigure with `-DPLATFORM=GLFW` or `-DPLATFORM=SDL`.
- If you hit issues with Intel drivers and texture arrays (notes in `little_engine.h`), prefer the current solution with multiple samplers (already implemented).
- For Linux: ensure you have OpenGL headers/drivers and X11/Wayland dev packages required by GLFW/SDL if using system builds (the repo vendors these, so it should mostly “just work”).

---

## 📦 Third‑party

Vendored in `thirdparty/` and built via CMake:
- **glad** (OpenGL loader)
- **glm** (math)
- **stb_image** (image loading)
- **freetype** (fonts)
- **imgui** (UI)
- **miniaudio** (audio)
- **glfw 3.3.2** and **SDL3** (pick one backend)
- **enet 1.3.17** (present but not enabled by default in CMake)

---

## 🗺️ Roadmap / Ideas

- Toggle networking (ENet) behind an option and wrap in a `Net/` module.
- More robust sprite batching and shape primitives.
- Sample game(s) and a project **template** (see below).

---

## 📁 Template vs Engine README

If you publish a **template repo** to start new games:
- **Engine README (this file):** documents modules, building the library, and API basics.
- **Template README (separate repo):** should be laser‑focused on *making a game*:
  - “Create a new project” steps (clone template, rename, set `PLATFORM`, build).
  - Where to put assets (`assets/`), how to add a scene/sprite, main loop example.
  - How to link against `LittleEngine` (as submodule or external dependency).
  - Simple “first run” checklist + common pitfalls.

I can generate a tailored **Template README** once you share how you want users to consume the engine (submodule vs. packaged library) and any starter code you prefer.

---

## 📝 License

The repository vendors multiple third‑party licenses under `thirdparty/`. If you plan to open‑source the engine itself, add your own top‑level `LICENSE` and clarify third‑party licenses.

