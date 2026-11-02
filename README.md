# JUJUTSU: CURSED DOMINION

A native Android action-RPG project scaffold using Kotlin for Android lifecycle integration and C++20 for the game runtime and OpenGL ES renderer.

## Current source scope

- Android landscape application shell.
- Kotlin activity and JNI bridge to a native C++ shared library.
- OpenGL ES 3.0 renderer smoke test.
- CMake and Android Gradle configuration.
- GitHub Actions workflow to compile a debug APK and publish it as a workflow artifact.

**This is a build foundation, not the completed game.** Combat, playable characters, 3D worlds, missions, progression, audio, cloud services and multiplayer are not implemented yet.

## Build an APK on GitHub

1. Open the repository's **Actions** tab.
2. Select **Android APK**.
3. Choose **Run workflow** to start a build manually, or push a commit to `main`.
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

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) and [docs/ROADMAP.md](docs/ROADMAP.md).

## Legal and creative direction

Use original characters, names, story, models, textures, music and audio unless appropriate licenses are obtained. The current scaffold contains no third-party game assets.

## Release builds

The workflow creates an unsigned development/debug APK. A distributable release requires a separately configured signing key stored as protected GitHub Actions secrets. Never commit signing keys or passwords.
