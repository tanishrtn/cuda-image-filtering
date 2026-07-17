#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>

extern "C" void launchBlur(unsigned char* input, unsigned char* output, int width, int height);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: image_filter <input-image> [output-image]\n");
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argc >= 3 ? argv[2] : "output.png";
    int width, height, channels;

    unsigned char* img = stbi_load(inputPath, &width, &height, &channels, 3);
    if (!img) {
        printf("FAILED: %s\n", stbi_failure_reason());
        getchar();
        return 1;
    }

    printf("Loaded image: %d x %d\n", width, height);

    int size = width * height * 3;
    unsigned char* output = (unsigned char*)malloc(size);

    launchBlur(img, output, width, height);

    stbi_write_png(outputPath, width, height, 3, output, width * 3);

    printf("Done. Check %s\n", outputPath);

    free(output);
    stbi_image_free(img);

    return 0;
}
