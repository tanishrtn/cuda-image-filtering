# CUDA Image Filtering

A portable C++17 image-blur application with an NVIDIA CUDA acceleration path, a CPU fallback, automatic backend selection, measurable execution time, and automated reference tests.

## What works

- Loads common image formats with `stb_image`
- Applies a boundary-safe 3×3 box blur to RGB pixels
- Uses 16×16 CUDA thread blocks when a compatible GPU is available
- Falls back to the same deterministic CPU algorithm on systems without CUDA
- Reports the selected backend and processing time
- Writes the filtered image as PNG
- Builds with CMake on Linux, macOS, and Windows

## Processing flow

```text
Input image -> RGB decode -> backend selection -> 3x3 blur -> PNG output
                                    |                |
                              CUDA kernel       CPU reference
```

## Build

Requires CMake 3.20+ and a C++17 compiler. CUDA is detected automatically.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

If the CUDA toolkit is installed, CMake builds the GPU implementation. Otherwise it produces a fully functional CPU build.

## Run

```bash
./build/image_filter input.pgm output.png
./build/image_filter input.jpg output.png --backend=cuda
./build/image_filter input.jpg output.png --backend=cpu
```

`--backend=auto` is the default. Explicitly requesting CUDA returns a clear error when no compatible device is available.

## Test

```bash
ctest --test-dir build --output-on-failure
```

The reference test checks exact pixel values for a known 2×2 RGB image. GitHub Actions builds the CPU fallback, executes the test, and processes the included sample image on every push and pull request.

## Repository map

```text
main.cpp                    CLI, image I/O, backend selection, timing
blur.cu                     CUDA kernel and checked memory operations
blur_cpu.cpp                Portable reference implementation
blur_stub.cpp               Non-CUDA build adapter
blur.h                      Shared interface
tests/test_blur_cpu.cpp     Deterministic pixel test
CMakeLists.txt              Portable conditional CUDA build
.github/workflows/ci.yml    Automated build and test
```

The older Visual Studio project files remain available for reference, but CMake is the supported build path.

## Scope and limitations

This project implements a box blur, not a full image-processing library. GPU performance depends on image size, transfer overhead, hardware, compiler settings, and warm-up behavior. No speedup is claimed without measuring both backends on the same system.
