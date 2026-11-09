#include "blur.h"

bool cudaAvailable() { return false; }

bool blurCuda(const unsigned char*, unsigned char*, int, int, int, std::string& error) {
    error = "This build does not include CUDA support";
    return false;
}
