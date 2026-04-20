#include <stdio.h>
#include <stdlib.h>

#include "mapping.h"
#include "mandelbrot.h"
#include "../BMP/bitmap.h"

/**
 * @file main.c
 * @brief Mandelbrot 图像生成程序的测试主函数。
 *
 * 本程序演示如何将以下三个模块连接起来：
 * - mapping：像素坐标到复平面坐标的映射；
 * - mandelbrot：Mandelbrot 集逃逸次数计算与颜色映射；
 * - bitmap：将像素缓冲区写成 BMP 文件。
 *
 * 程序功能：
 * 1. 从命令行读取输出文件名、观察窗口中心和窗口半宽；
 * 2. 初始化映射参数和 Mandelbrot 参数；
 * 3. 遍历整张图像的每个像素；
 * 4. 将像素映射到复平面点；
 * 5. 计算逃逸迭代次数并映射为颜色；
 * 6. 把结果写入 BMP 文件。
 *
 * 命令行格式：
 * @code
 * ./mandelbrot_test output.bmp center_x center_y half_width
 * @endcode
 *
 * 例如：
 * @code
 * ./mandelbrot_test mandelbrot.bmp -0.5 0.0 1.5
 * @endcode
 */

int main(int argc, char *argv[])
{
    const char *output_file;
    double center_x;
    double center_y;
    double half_width;

    int width;
    int height;
    int max_iterations;
    double escape_radius;

    Mapping map;
    Mandelbrot mb;

    unsigned char *image;
    int px;
    int py;
    int pos;
    int ret;

    if (argc != 5)
    {
        fprintf(stderr,
                "Usage: %s output.bmp center_x center_y half_width\n",
                argv[0]);
        fprintf(stderr,
                "Example: %s mandelbrot.bmp -0.5 0.0 1.5\n",
                argv[0]);
        return 1;
    }

    output_file = argv[1];
    center_x = atof(argv[2]);
    center_y = atof(argv[3]);
    half_width = atof(argv[4]);

    /**
     * 这里选取一个适中的分辨率，便于测试：
     * - 宽 800
     * - 高 600
     *
     * 对教学演示来说，这个大小足够清楚，
     * 同时生成速度也比较合适。
     */
    width = 800;
    height = 600;

    /**
     * Mandelbrot 计算参数：
     * - max_iterations 越大，边界细节越丰富；
     * - escape_radius 经典取值通常为 2.0。
     */
    max_iterations = 500;
    escape_radius = 2.0;

    mapping_init(&map, width, height, center_x, center_y, half_width);
    mandelbrot_init(&mb, max_iterations, escape_radius);

    image = (unsigned char *)malloc(width * height * 3);
    if (image == NULL)
    {
        fprintf(stderr, "Error: malloc failed.\n");
        return 1;
    }

    /**
     * 按“从上到下、从左到右”的顺序遍历像素。
     *
     * 这正是 bitmap 模块所要求的输入缓冲区排列方式：
     * - 每个像素 3 字节；
     * - 字节顺序为 B, G, R；
     * - 行顺序为从上到下。
     */
    for (py = 0; py < height; ++py)
    {
        for (px = 0; px < width; ++px)
        {
            ComplexPoint c;
            int iterations;
            RgbColor color;

            c = mapping_pixel_to_complex(&map, px, py);
            iterations = mandelbrot_escape_iterations(&mb, c.x, c.y);
            color = mandelbrot_color(&mb, iterations);

            pos = (py * width + px) * 3;

            /**
             * 注意 bitmap.h 的约定：
             * 原始像素数组中每个像素的字节顺序必须是 B, G, R。
             */
            image[pos + 0] = color.b;
            image[pos + 1] = color.g;
            image[pos + 2] = color.r;
        }
    }

    ret = build_bmp(output_file, width, height, image);
    if (ret != 0)
    {
        fprintf(stderr, "Error: build_bmp failed, ret = %d\n", ret);
        free(image);
        return 1;
    }

    free(image);

    printf("BMP file generated successfully: %s\n", output_file);
    printf("Image size      : %d x %d\n", width, height);
    printf("Center          : (%f, %f)\n", center_x, center_y);
    printf("Half width      : %f\n", half_width);
    printf("Max iterations  : %d\n", max_iterations);
    printf("Escape radius   : %f\n", escape_radius);

    return 0;
}