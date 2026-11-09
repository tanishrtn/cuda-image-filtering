#pragma once

#include <cstddef>
#include <string>

void blurCpu(const unsigned char* input, unsigned char* output, int width, int height, int channels);
bool cudaAvailable();
bool blurCuda(const unsigned char* input, unsigned char* output, int width, int height, int channels, std::string& error);
