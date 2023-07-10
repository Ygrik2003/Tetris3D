# Tetris3D
----
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
