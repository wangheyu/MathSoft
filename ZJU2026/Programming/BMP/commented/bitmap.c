#include "bitmap.h"
#include <stdio.h>

/**
 * @file bitmap.c
 * @brief BMP 文件写出函数的实现
 *
 * 本文件实现 `build_bmp()`，负责将内存中的原始像素数组
 * 写成 24 位无压缩 BMP 文件。
 */

/**
 * @brief 将原始像素数据写入 BMP 文件
 *
 * 本实现主要完成以下工作：
 * - 检查输入参数；
 * - 计算每行的 4 字节对齐补齐量；
 * - 填写 BMP 文件头和信息头；
 * - 按 BMP 规定的顺序写入像素数据。
 *
 * 具体的参数约定、像素格式和错误码定义见 `bitmap.h`。
 *
 * @note BMP 要求每一行数据按 4 字节对齐，因此行末可能需要补 0。
 * @note 当 `biHeight > 0` 时，BMP 使用自底向上的存储方式，
 *       因此写文件时需要从最后一行开始写。
 */
int build_bmp(const char *file, int w, int h, const unsigned char *image)
{
    /** 文件指针，用于写二进制 BMP 文件。 */
    FILE *fp;

    /** BMP 文件头。 */
    BITMAPFILEHEADER file_header;

    /** BMP 信息头。 */
    BITMAPINFOHEADER info_header;

    /** 每行像素数据的原始字节数，不含补齐字节。 */
    int row_size_no_pad;

    /** 为满足 4 字节对齐而需要补写的字节数。 */
    int padding;

    /** 每行实际写入文件的总字节数。 */
    int row_size_with_pad;

    /** 行循环变量。 */
    int y;

    /** 用于写入行末补齐字节的缓冲区，最多补 3 个字节。 */
    unsigned char pad[3] = {0, 0, 0};

    /** 检查参数是否合法。 */
    if (file == NULL || image == NULL || w <= 0 || h <= 0)
    {
        return -1;
    }

    /** 以二进制写模式打开输出文件。 */
    fp = fopen(file, "wb");
    if (fp == NULL)
    {
        return -2;
    }

    /**
     * 计算每行像素数据大小以及对齐补齐量。
     *
     * 对 24 位 BMP：
     * - 每个像素占 3 字节；
     * - 每行必须补齐到 4 字节的整数倍。
     */
    row_size_no_pad   = w * 3;
    padding           = (4 - (row_size_no_pad % 4)) % 4;
    row_size_with_pad = row_size_no_pad + padding;

    /** 填写 BMP 信息头。 */
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

    /** 填写 BMP 文件头。 */
    file_header.bfType = 0x4D42;
    file_header.bfOffBits =
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    file_header.bfSize =
        file_header.bfOffBits + info_header.biSizeImage;
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;

    /** 写入 BMP 文件头。 */
    if (fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, fp) != 1)
    {
        fclose(fp);
        return -3;
    }

    /** 写入 BMP 信息头。 */
    if (fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, fp) != 1)
    {
        fclose(fp);
        return -3;
    }

    /**
     * 逐行写入像素数据。
     *
     * 由于这里使用正的 `biHeight`，
     * BMP 文件要求按“自底向上”的顺序存储，
     * 因此必须从最后一行向第一行倒序写出。
     */
    for (y = h - 1; y >= 0; --y)
    {
        /** 当前行像素数据在输入缓冲区中的起始地址。 */
        const unsigned char *row = image + y * row_size_no_pad;

        /** 写入当前行的实际像素数据。 */
        if (fwrite(row, 1, row_size_no_pad, fp) != (size_t)row_size_no_pad)
        {
            fclose(fp);
            return -3;
        }

        /** 如有需要，在行末补写对齐字节。 */
        if (padding > 0)
        {
            if (fwrite(pad, 1, padding, fp) != (size_t)padding)
            {
                fclose(fp);
                return -3;
            }
        }
    }

    /** 关闭输出文件。 */
    fclose(fp);

    return 0;
}