#include "blur.h"

void blurCpu(const unsigned char* input, unsigned char* output, int width, int height, int channels) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
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
    }
}
