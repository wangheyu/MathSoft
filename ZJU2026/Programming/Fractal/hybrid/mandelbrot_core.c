#include <math.h>
#include <stdint.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT void mandelbrot_compute(
    int width,
    int height,
    double center_x,
    double center_y,
    double view_width,
    double view_height,
    int max_iter,
    double* smooth,
    uint8_t* inside_mask
) {
    double x_min = center_x - view_width / 2.0;
    double x_max = center_x + view_width / 2.0;
    double y_min = center_y - view_height / 2.0;
    double y_max = center_y + view_height / 2.0;

    for (int py = 0; py < height; py++) {
        double y = y_min + (y_max - y_min) * py / (height - 1);

        for (int px = 0; px < width; px++) {
            double x = x_min + (x_max - x_min) * px / (width - 1);

            double cr = x;
            double ci = y;
            double zr = 0.0;
            double zi = 0.0;

            int iter = 0;
            double zr2 = 0.0;
            double zi2 = 0.0;

            while (iter < max_iter && (zr2 + zi2) <= 4.0) {
                zi = 2.0 * zr * zi + ci;
                zr = zr2 - zi2 + cr;
                zr2 = zr * zr;
                zi2 = zi * zi;
                iter++;
            }

            int idx = py * width + px;

            if (iter >= max_iter) {
                smooth[idx] = 0.0;
                inside_mask[idx] = 1;
            } else {
                double mag = sqrt(zr2 + zi2);

                // 平滑着色
                double value = iter + 1 - log(log(mag)) / log(2.0);

                smooth[idx] = value;
                inside_mask[idx] = 0;
            }
        }
    }
}