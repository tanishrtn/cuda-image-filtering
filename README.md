# CUDA Image Filtering

A C++ and CUDA implementation of a GPU-accelerated 3x3 box-blur filter. The CUDA kernel processes each output pixel in parallel and writes the filtered result as a PNG.

## Tech stack

- C++
- NVIDIA CUDA
- `stb_image` / `stb_image_write`

## How it works

1. Loads an input image as RGB data.
2. Copies the image to GPU memory.
3. Runs a CUDA kernel with 16x16 thread blocks.
4. Applies a 3x3 box blur with boundary checks.
5. Copies the result back to the CPU and writes a PNG file.

## Build and run

Open `image.slnx` in Visual Studio with CUDA support, then build the project. From the executable directory:

```bash
image_filter input.jpg output.png
```

> An NVIDIA GPU and a CUDA-enabled Visual Studio toolchain are required to build and run this project.

## Notes

This repository contains the CUDA implementation currently available in the project source. It does not include MPI/distributed-processing code.
