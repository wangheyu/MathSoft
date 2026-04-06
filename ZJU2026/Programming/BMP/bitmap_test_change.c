#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

int main(void)
{
    int w = 256;
    int h = 256;
    int x, y;
    unsigned char *image;
    int ret;

    image = (unsigned char *)malloc(w * h * 3);
    if (image == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    for (y = 0; y < h; ++y)
    {
        for (x = 0; x < w; ++x)
        {
            int pos = (y * w + x) * 3;

            image[pos + 0] = (unsigned char)x;
            image[pos + 1] = (unsigned char)y;
            image[pos + 2] = (unsigned char)((x + y) / 2);
        }
    }

    ret = build_bmp("test.bmp", w, h, image);
    if (ret != 0)
    {
        fprintf(stderr, "build_bmp failed, ret = %d\n", ret);
        free(image);
        return 1;
    }

    printf("BMP file generated successfully: test.bmp\n");

    free(image);
    return 0;
}