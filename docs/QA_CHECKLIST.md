# Prototype QA checklist

Run these checks on at least one physical Android device and one emulator where possible.

## Build and launch
- [ ] GitHub Actions completes the Android APK job.
- [ ] Downloaded debug APK installs on an Android 8.0+ device.
- [ ] App opens in landscape without a crash.
- [ ] OpenGL ES renderer displays the arena, player, enemies and touch-control visuals.

## Touch input
- [ ] Dragging on the left side moves the player in all four directions.
- [ ] Releasing the left-side touch stops movement.
- [ ] Tapping the lower-right area triggers a strike.
- [ ] Rapid taps respect the cooldown meter.
- [ ] Touching the top HUD does not unexpectedly trigger a strike.

## Arena loop
- [ ] Enemies approach the player.
- [ ] Enemy contact reduces the health bar.
- [ ] A strike only damages enemies within its range.
- [ ] Defeating all enemies starts the next wave.
- [ ] Player health reaching zero resets the encounter and restores health.
- [ ] HUD wave and kill counters update during play.

## Lifecycle and performance
- [ ] Rotating is handled by the fixed landscape orientation.
- [ ] Backgrounding and resuming the app restores rendering.
- [ ] No native crash occurs after several minutes of play.
- [ ] Test on lower-end hardware and record frame rate and thermal behavior.

## Known limitations
- This is a geometric 2D arena prototype, not a 3D game.
- Touch input currently targets a single active pointer.
- No save system, sound, character animation, story missions or multiplayer exists yet.
- A successful CI build does not replace physical-device testing.
