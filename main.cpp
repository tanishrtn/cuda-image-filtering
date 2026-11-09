#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "blur.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

enum class Backend { Auto, Cpu, Cuda };

bool loadAsciiPgm(const char* path, std::vector<unsigned char>& rgb, int& width, int& height) {
    std::ifstream stream(path);
    std::string magic;
    if (!(stream >> magic) || magic != "P2") return false;

    auto skipComments = [&stream]() {
        while (stream >> std::ws && stream.peek() == '#') {
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    };
    skipComments();
    int maximum = 0;
    if (!(stream >> width >> height)) return false;
    skipComments();
    if (!(stream >> maximum) || width <= 0 || height <= 0 || maximum <= 0) return false;

    rgb.resize(static_cast<std::size_t>(width) * height * 3);
    for (int pixel = 0; pixel < width * height; ++pixel) {
        int value = 0;
        if (!(stream >> value) || value < 0 || value > maximum) return false;
        const auto scaled = static_cast<unsigned char>((value * 255) / maximum);
        rgb[pixel * 3] = scaled;
        rgb[pixel * 3 + 1] = scaled;
        rgb[pixel * 3 + 2] = scaled;
    }
    return true;
}

Backend parseBackend(const std::string& option) {
    if (option == "--backend=cpu") return Backend::Cpu;
    if (option == "--backend=cuda") return Backend::Cuda;
    if (option == "--backend=auto") return Backend::Auto;
    throw std::invalid_argument("backend must be auto, cpu, or cuda");
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: image_filter <input-image> [output.png] [--backend=auto|cpu|cuda]\n";
        return 2;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argc >= 3 && std::string(argv[2]).rfind("--backend=", 0) != 0 ? argv[2] : "output.png";
    const std::string backendOption = std::string(argv[argc - 1]).rfind("--backend=", 0) == 0 ? argv[argc - 1] : "--backend=auto";

    Backend requested;
    try {
        requested = parseBackend(backendOption);
    } catch (const std::invalid_argument& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }

    int width = 0;
    int height = 0;
    int sourceChannels = 0;
    constexpr int channels = 3;
    std::vector<unsigned char> asciiPgm;
    unsigned char* image = stbi_load(inputPath, &width, &height, &sourceChannels, channels);
    const bool imageOwnedByStb = image != nullptr;
    if (!image) {
        if (!loadAsciiPgm(inputPath, asciiPgm, width, height)) {
            std::cerr << "Unable to load '" << inputPath << "': " << stbi_failure_reason() << '\n';
            return 1;
        }
        image = asciiPgm.data();
    }

    const std::size_t bytes = static_cast<std::size_t>(width) * height * channels;
    std::vector<unsigned char> output(bytes);
    bool usedCuda = requested != Backend::Cpu && cudaAvailable();
    if (requested == Backend::Cuda && !usedCuda) {
        std::cerr << "CUDA was requested, but no compatible CUDA device is available.\n";
        if (imageOwnedByStb) stbi_image_free(image);
        return 1;
    }

    const auto started = std::chrono::steady_clock::now();
    if (usedCuda) {
        std::string error;
        if (!blurCuda(image, output.data(), width, height, channels, error)) {
            std::cerr << "CUDA processing failed: " << error << '\n';
            if (imageOwnedByStb) stbi_image_free(image);
            return 1;
        }
    } else {
        blurCpu(image, output.data(), width, height, channels);
    }
    const auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();

    const int wrote = stbi_write_png(outputPath, width, height, channels, output.data(), width * channels);
    if (imageOwnedByStb) stbi_image_free(image);
    if (!wrote) {
        std::cerr << "Unable to write '" << outputPath << "'.\n";
        return 1;
    }

    std::cout << "Processed " << width << 'x' << height << " image with "
              << (usedCuda ? "CUDA" : "CPU fallback") << " in " << elapsed << " ms.\n"
              << "Wrote " << outputPath << '\n';
    return 0;
}
