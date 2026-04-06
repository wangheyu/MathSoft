#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

/**
 * @file bitmap_test.c
 * @brief `build_bmp()` 的测试程序
 *
 * 本程序在内存中生成一张 256x256 的彩色渐变图，
 * 然后调用 `build_bmp()` 将其写入 BMP 文件。
 */

/**
 * @brief 生成测试图像并输出 BMP 文件
 *
 * 本程序完成以下工作：
 * - 为整张图像分配像素缓冲区；
 * - 按预定规则生成一张彩色渐变图；
 * - 调用 `build_bmp()` 将像素数据写入 `test.bmp`；
 * - 在结束前释放动态申请的内存。
 *
 * @return 0 表示程序正常结束。
 * @return 1 表示内存申请失败或 BMP 文件生成失败。
 */
int main(void)
{
    /** 图像宽度，单位为像素。 */
    int w = 256;

    /** 图像高度，单位为像素。 */
    int h = 256;

    /** 像素坐标循环变量。 */
    int x, y;

    /** 图像像素缓冲区首地址。 */
    unsigned char *image;

    /** 保存 `build_bmp()` 的返回值。 */
    int ret;

    /**
     * 为整张图像分配连续内存。
     *
     * 每个像素占 3 个字节，因此总字节数为 `w * h * 3`。
     */
    image = (unsigned char *)malloc(w * h * 3);

    /** 检查内存申请是否成功。 */
    if (image == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    /**
     * 逐像素生成测试图像。
     *
     * 这里构造的是一张简单的彩色渐变图：
     * - 蓝色分量随 `x` 增大；
     * - 绿色分量随 `y` 增大；
     * - 红色分量取 `x` 与 `y` 的平均值。
     */
    for (y = 0; y < h; ++y)
    {
        for (x = 0; x < w; ++x)
        {
            /** 当前像素在一维字节数组中的起始下标。 */
            int pos = (y * w + x) * 3;

            image[pos + 0] = (unsigned char)x;
            image[pos + 1] = (unsigned char)y;
            image[pos + 2] = (unsigned char)((x + y) / 2);
        }
    }

    /** 调用 `build_bmp()` 输出测试 BMP 文件。 */
    ret = build_bmp("test.bmp", w, h, image);

    /** 若生成失败，则输出错误信息并释放内存。 */
    if (ret != 0)
    {
        fprintf(stderr, "build_bmp failed, ret = %d\n", ret);
        free(image);
        return 1;
    }

    /** 输出成功提示信息。 */
    printf("BMP file generated successfully: test.bmp\n");

    /** 释放动态申请的内存。 */
    free(image);

    return 0;
}