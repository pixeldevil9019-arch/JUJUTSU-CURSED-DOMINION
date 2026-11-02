# Technical architecture

## Implemented runtime

- Android host: Kotlin Activity owns the landscape lifecycle and OpenGL ES surface.
- Input: Android touch events are normalized and passed to the native runtime over JNI.
- Native runtime: C++20 shared library loaded with JNI.
- Renderer: OpenGL ES 3.0 draws a simple top-down arena, player/enemy shapes, controls and health bars.
- Simulation: bounded frame delta, player movement, enemy pursuit, contact damage, cooldown-based strike, wave progression and respawn.
- Build: Android Gradle Plugin, Gradle, Android NDK and CMake.

## Current prototype boundary

The current game is a small 2D arena vertical slice using geometric placeholders. It is deliberately not presented as a completed 3D action RPG. No 3D model loader, skeletal animation, persistent save system, audio pipeline, story mission system, backend or network play is implemented yet.

## Planned module boundaries

| Module | Responsibility | Status |
|---|---|---|
| platform/android | Activity, lifecycle, touch input | Implemented prototype |
| engine/core | Timing, events, memory and application loop | Basic frame timing only |
| renderer | GPU resources, materials, scene rendering | OpenGL ES primitive renderer |
| gameplay/characters | Movement and character state | Basic arena movement |
| gameplay/combat | Hit detection, damage and cooldowns | Basic proximity strike |
| gameplay/ai | Enemy decision making | Simple pursue behavior |
| world | Streaming, 3D environments and weather | Planned |
| missions/progression | Objectives, rewards and saves | Planned |
| audio | Music, effects and ambience | Planned |
| backend/multiplayer | Optional online services | Future |

## Engineering constraints

- Keep the first production playable release offline and single-player.
- Use bounded simulation steps and explicit ownership for native resources.
- Keep JNI input thread-safe; protect shared simulation state.
- Add device capability checks before enabling expensive rendering features.
- Use level of detail, texture compression, asset streaming and object pooling as content grows.
- Use original or properly licensed assets. No copyrighted franchise assets are included in this scaffold.
