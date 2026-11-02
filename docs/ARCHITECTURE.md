# Technical architecture

## Initial runtime

- Android host: Kotlin Activity owns the Android lifecycle and an OpenGL ES surface.
- Native runtime: C++20 shared library loaded with JNI.
- Renderer: OpenGL ES 3.0 baseline; Vulkan may be evaluated later as an optional backend.
- Build: Android Gradle Plugin, Gradle, Android NDK and CMake.
- Asset interchange: glTF 2.0 is the planned model format.

## Planned module boundaries

| Module | Responsibility | Status |
|---|---|---|
| platform/android | Activity, lifecycle, input and Android services | Bootstrap only |
| engine/core | Time step, events, memory and application loop | Planned |
| renderer | GPU resources, materials, shaders and scene rendering | Triangle smoke test only |
| gameplay/characters | Character state, movement and abilities | Planned |
| gameplay/combat | Hit detection, damage, stamina and energy resource | Planned |
| gameplay/ai | Enemy decision making and boss logic | Planned |
| world | Streaming, colony scenes, weather and time of day | Planned |
| missions | Objectives, encounters and rewards | Planned |
| progression | Experience, unlocks and save data | Planned |
| audio | Music, effects and ambience | Planned |
| backend | Optional account and cloud progression services | Future |
| multiplayer | Networked cooperative/competitive play | Future |

## Engineering constraints

- Keep the first playable release offline and single-player.
- Use bounded simulation steps and explicit ownership for native resources.
- Add device capability checks before enabling expensive rendering features.
- Use level of detail, texture compression, asset streaming and object pooling as content grows.
- Do not claim a subsystem is implemented until code and tests demonstrate it.
- Use original or properly licensed assets. No copyrighted franchise assets are included in this scaffold.
