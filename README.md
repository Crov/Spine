# Spine

Spine is a small scripting language for describing tasks and workflows.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

## Run

Run a complete script:

```powershell
.\build\Debug\spine.exe .\scripts\test.spine
```

Run a named task:

```powershell
.\build\Debug\spine.exe .\scripts\release.spine deploy
```
