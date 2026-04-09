#include <stddef.h>

/*
 * 计算给定视野下 Mandelbrot 集的逃逸次数。
 *
 * 参数：
 *   width, height : 图像宽高（像素）
 *   xmin, xmax    : 复平面实部范围
 *   ymin, ymax    : 复平面虚部范围
 *   max_iter      : 最大迭代次数
 *   output        : 输出数组，长度至少为 width * height
 *
 * 输出：
 *   output[py * width + px] = 对应像素点的逃逸次数
 */
void mandelbrot_escape_counts(
    int width,
    int height,
    double xmin,
    double xmax,
    double ymin,
    double ymax,
    int max_iter,
    int *output
) {
    int px, py;

    for (py = 0; py < height; ++py) {
        double cy;

        if (height > 1) {
            cy = ymax - ((double)py / (double)(height - 1)) * (ymax - ymin);
        } else {
            cy = ymax;
        }

        for (px = 0; px < width; ++px) {
            double cx;
            double zx, zy;
            int k;

            if (width > 1) {
                cx = xmin + ((double)px / (double)(width - 1)) * (xmax - xmin);
            } else {
                cx = xmin;
            }

            zx = 0.0;
            zy = 0.0;
            k = 0;

            while ((zx * zx + zy * zy <= 4.0) && (k < max_iter)) {
                double new_zx = zx * zx - zy * zy + cx;
                double new_zy = 2.0 * zx * zy + cy;

                zx = new_zx;
                zy = new_zy;
                ++k;
            }

            output[py * width + px] = k;
        }
    }
}