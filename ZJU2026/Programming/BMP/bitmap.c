#include "bitmap.h"
#include <stdio.h>

int build_bmp(const char *file, int w, int h, const unsigned char *image)
{
    FILE *fp;
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    int row_size_no_pad;
    int padding;
    int row_size_with_pad;
    int y;
    unsigned char pad[3] = {0, 0, 0};

    if (file == NULL || image == NULL || w <= 0 || h <= 0)
        return -1;

    fp = fopen(file, "wb");
    if (fp == NULL)
        return -2;

    row_size_no_pad = w * 3;
    padding = (4 - (row_size_no_pad % 4)) % 4;
    row_size_with_pad = row_size_no_pad + padding;

    info_header.biSize = sizeof(BITMAPINFOHEADER);
    info_header.biWidth = w;
    info_header.biHeight = h;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24;
    info_header.biCompression = 0;
    info_header.biSizeImage = row_size_with_pad * h;
    info_header.biXPelsPerMeter = 2834;
    info_header.biYPelsPerMeter = 2834;
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;

    file_header.bfType = 0x4D42;
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    file_header.bfSize = file_header.bfOffBits + info_header.biSizeImage;
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;

    if (fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, fp) != 1)
    {
        fclose(fp);
        return -3;
    }

    if (fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, fp) != 1)
    {
        fclose(fp);
        return -3;
    }

    for (y = h - 1; y >= 0; --y)
    {
        const unsigned char *row = image + y * row_size_no_pad;

        if (fwrite(row, 1, row_size_no_pad, fp) != (size_t)row_size_no_pad)
        {
            fclose(fp);
            return -3;
        }

        if (padding > 0)
        {
            if (fwrite(pad, 1, padding, fp) != (size_t)padding)
            {
                fclose(fp);
                return -3;
            }
        }
    }

    fclose(fp);
    return 0;
}