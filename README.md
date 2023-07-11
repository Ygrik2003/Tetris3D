# Tetris3D
----
## Build
For use SDL3 and Assimp:
```bash
git submodule init
git submodule update
```
For debug:
```bash
cmake --preset CMakePresets.json --preset=ninja-debug
```

For release:
```bash
cmake --preset CMakePresets.json --preset=ninja-release
```

For build for Windows from Linux:
```bash
cmake --preset CMakePresets.json --preset=ninja-windows-release
```

For compilation for android use Android Studio from folder "android-project".

## Gameplay
On PC use WASD for moving, and left, right and down arrows for rotating.

On Android use buttons.

Also you can rotate camera with holding LBM on PC or swipe on Android.
