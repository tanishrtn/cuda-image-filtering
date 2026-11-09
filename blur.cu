#include "blur.h"

#include <cuda_runtime.h>

namespace {

__global__ void blurKernel(const unsigned char* input, unsigned char* output, int width, int height, int channels) {
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;

    for (int channel = 0; channel < channels; ++channel) {
        int sum = 0;
        int count = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                const int nx = x + dx;
                const int ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    sum += input[(ny * width + nx) * channels + channel];
                    ++count;
                }
            }
        }
        output[(y * width + x) * channels + channel] = static_cast<unsigned char>(sum / count);
    }
}

bool check(cudaError_t status, const char* operation, std::string& error) {
    if (status == cudaSuccess) return true;
    error = std::string(operation) + ": " + cudaGetErrorString(status);
    return false;
}

}  // namespace

bool cudaAvailable() {
    int deviceCount = 0;
    return cudaGetDeviceCount(&deviceCount) == cudaSuccess && deviceCount > 0;
}

bool blurCuda(const unsigned char* input, unsigned char* output, int width, int height, int channels, std::string& error) {
    const std::size_t size = static_cast<std::size_t>(width) * height * channels;
    unsigned char* deviceInput = nullptr;
    unsigned char* deviceOutput = nullptr;

    if (!check(cudaMalloc(&deviceInput, size), "cudaMalloc input", error)) return false;
    if (!check(cudaMalloc(&deviceOutput, size), "cudaMalloc output", error)) {
        cudaFree(deviceInput);
        return false;
    }

    bool ok = check(cudaMemcpy(deviceInput, input, size, cudaMemcpyHostToDevice), "copy input to GPU", error);
    if (ok) {
        const dim3 block(16, 16);
        const dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
        blurKernel<<<grid, block>>>(deviceInput, deviceOutput, width, height, channels);
        ok = check(cudaGetLastError(), "launch blur kernel", error) &&
             check(cudaDeviceSynchronize(), "synchronize blur kernel", error) &&
             check(cudaMemcpy(output, deviceOutput, size, cudaMemcpyDeviceToHost), "copy output to CPU", error);
    }

    cudaFree(deviceInput);
    cudaFree(deviceOutput);
    return ok;
}
