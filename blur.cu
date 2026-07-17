#include <cuda_runtime.h>

__global__ void blurKernel(unsigned char* input, unsigned char* output,
    int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    for (int c = 0; c < 3; c++) {

        int sum = 0;
        int count = 0;

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {

                int nx = x + dx;
                int ny = y + dy;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int idx = (ny * width + nx) * 3 + c;
                    sum += input[idx];
                    count++;
                }
            }
        }

        int outIdx = (y * width + x) * 3 + c;
        output[outIdx] = sum / count;
    }
}

extern "C" void launchBlur(unsigned char* input, unsigned char* output,
    int width, int height)
{
    unsigned char* d_in;
    unsigned char* d_out;

    size_t size = (size_t)width * height * 3;

    cudaMalloc((void**)&d_in, size);
    cudaMalloc((void**)&d_out, size);

    cudaMemcpy(d_in, input, size, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((width + 15) / 16, (height + 15) / 16);


    blurKernel<<<grid, block>>>(d_in, d_out, width, height);

    cudaDeviceSynchronize();

    cudaMemcpy(output, d_out, size, cudaMemcpyDeviceToHost);

    cudaFree(d_in);
    cudaFree(d_out);
}