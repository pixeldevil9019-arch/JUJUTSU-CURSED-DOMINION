# JUJUTSU: CURSED DOMINION

An original supernatural action-RPG prototype for Android, built with Kotlin, C++20, JNI and OpenGL ES 3.0.

## Playable prototype included

The current build is an **offline 2D arena vertical slice**:
- Landscape Android application with an OpenGL ES render loop.
- Touch joystick movement and a separate strike button.
- Four pursuing enemy targets, basic proximity damage and enemy health.
- Player health, wave count, kill count and attack cooldown HUD.
- Automatic next-wave spawning and player respawn.
- Native C++ simulation and GPU-rendered geometric placeholder art.

This is a functional prototype milestone, **not the finished 3D RPG**. It does not yet include 3D character models, skeletal animation, a large open world, story missions, character selection, inventory, save progression, audio, online accounts or multiplayer. The arena art is intentionally made from simple shapes while the gameplay loop is being tested.

## Controls

- Hold and drag on the **left half** of the screen to move.
- Tap the **right/lower half** to use Cursed Strike.
- Move close to enemies before striking; the strike has a short cooldown.
- Avoid contact with enemies. Defeating the wave advances to the next one.

See [Prototype controls](docs/PROTOTYPE_CONTROLS.md).

## Build an APK on GitHub

1. Open the repository's **Actions** tab.
2. Select **Android APK**.
3. Choose **Run workflow**, or push a commit to `main`.
4. Open the completed workflow run.
5. Download **cursed-dominion-debug-apk** from its Artifacts section.

Pull requests targeting `main` also run the build as a validation check.

## Local build requirements

- JDK 17
- Android SDK Platform 35
- Android NDK 27.2.12479018
- CMake 3.22.1
- Gradle 8.9

Run `gradle --no-daemon :app:assembleDebug`.

APK output: `app/build/outputs/apk/debug/app-debug.apk`.

## Planned production architecture

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/ROADMAP.md](docs/ROADMAP.md). The next milestones are a real 3D camera/mesh pipeline, original character assets, collision-aware movement, combat state/data, and a first small 3D environment.

## Legal and creative direction

Use original characters, names, story, models, textures, music and audio unless appropriate licenses are obtained. No third-party anime/game assets are included in this repository.

## Release builds

The workflow creates a development/debug APK. A distributable release requires a separately configured signing key stored as protected GitHub Actions secrets. Never commit signing keys or passwords.
