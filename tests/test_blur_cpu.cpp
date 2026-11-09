#include "blur.h"

#include <array>
#include <cstdlib>
#include <iostream>

int main() {
    const std::array<unsigned char, 12> input = {
        0, 0, 0,
        30, 30, 30,
        60, 60, 60,
        90, 90, 90
    };
    std::array<unsigned char, 12> output{};
    blurCpu(input.data(), output.data(), 2, 2, 3);

    for (const unsigned char value : output) {
        if (value != 45) {
            std::cerr << "Expected every channel to equal 45, got " << static_cast<int>(value) << '\n';
            return EXIT_FAILURE;
        }
    }
    std::cout << "CPU blur reference test passed\n";
    return EXIT_SUCCESS;
}
