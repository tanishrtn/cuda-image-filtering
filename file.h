#define _CRT_SECURE_NO_WARNINGS
#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int width, height;
    unsigned char* data;
} Image;


Image readPGM(const char* filename) {
    Image img;

    img.width = 256;
    img.height = 256;

    int size = img.width * img.height * 3;
    img.data = (unsigned char*)malloc(size);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {

            int idx = (y * img.width + x) * 3;

            img.data[idx + 0] = (x * 255) / img.width;
            img.data[idx + 1] = (y * 255) / img.height;
            img.data[idx + 2] = 128;
        }
    }

    return img;
}


void writePPM(const char* filename, Image img) {
    FILE* f = fopen(filename, "wb");

    fprintf(f, "P6\n%d %d\n255\n", img.width, img.height);
    fwrite(img.data, 1, img.width * img.height * 3, f);

    fclose(f);
}

#endif