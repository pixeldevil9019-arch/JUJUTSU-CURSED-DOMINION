# Development roadmap

## Completed in repository

- **Phase 1 — Android build foundation:** Kotlin/JNI bridge, C++20 OpenGL ES renderer and automated debug APK workflow.
- **Phase 2 prototype — Offline arena vertical slice:** touch movement, player health, enemy pursuit, proximity strike, cooldown, kill count and wave spawning.

## Remaining milestones

1. **Phase 3 — 3D rendering foundation:** camera, perspective, depth testing, mesh/material pipeline, glTF loading and device quality settings.
2. **Phase 4 — Original playable character:** model, rig, animation state machine, movement and third-person camera.
3. **Phase 5 — Combat system:** attack combos, hitboxes, enemy stagger, energy resource, cooldown tuning and combat feedback.
4. **Phase 6 — First 3D environment:** compact original urban district, collision, navigation and streaming.
5. **Phase 7 — Enemies and missions:** enemy archetypes, encounter flow, objectives and boss prototype.
6. **Phase 8 — Progression:** save data, experience, unlocks and customization.
7. **Phase 9 — Content expansion:** additional original characters, environments, abilities, UI and audio.
8. **Phase 10 — Online foundations:** only if needed; evaluate authentication and cloud saves separately from offline play.
9. **Phase 11 — Optimization and release:** device profiling, accessibility, QA, signing and release packaging.

Each milestone needs a build check and a clear implemented-versus-pending report. A debug APK build passing does not by itself establish device compatibility or production readiness.
