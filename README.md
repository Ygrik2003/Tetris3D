# Tetris3D
----
```shaell
git submodule init
git submodule update
For debug:
```shaell
cmake --preset CMakePresets.json --preset=ninja-debug
```

For release:
```shaell
cmake --preset CMakePresets.json --preset=ninja-release
```

For build for Windows from Linux:
```shaell
cmake --preset CMakePresets.json --preset=ninja-windows-release
```

For compilation for android use Android Studio from folder "android-project".
