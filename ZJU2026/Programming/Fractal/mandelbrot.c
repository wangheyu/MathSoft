#include "mandelbrot.h"

#include <math.h>

/**
 * @file mandelbrot.c
 * @brief Mandelbrot 集迭代计算与颜色映射函数的实现。
 *
 * 本文件实现如下功能：
 * - 初始化 Mandelbrot 计算参数；
 * - 计算复平面点 c = cx + i cy 的逃逸迭代次数；
 * - 根据逃逸迭代次数生成 RGB 颜色。
 *
 * 设计原则：
 * - 代码尽量直观，优先服务于教学演示；
 * - 不追求复杂优化；
 * - 将“数学计算”和“颜色映射”集中放在同一模块中，
 *   便于学生理解“数值结果如何变成图像颜色”。
 *
 * 注意：
 * 本文件返回的颜色使用 RgbColor 结构，成员顺序是 r、g、b。
 * 但位图输出函数 build_bmp() 需要的原始像素缓冲区顺序是 B、G、R，
 * 因此主程序在写入图像缓存时应自行调整字节顺序。
 */

/**
 * @brief 初始化 Mandelbrot 参数结构。
 *
 * 该函数将用户给定的最大迭代次数和逃逸半径写入结构体。
 *
 * @param mb 指向待初始化 Mandelbrot 结构体的指针。
 * @param max_iterations 最大迭代次数。
 * @param escape_radius 逃逸半径，经典 Mandelbrot 图通常取 2.0。
 *
 * @note
 * 若传入空指针，本函数直接返回，不做任何操作。
 *
 * @note
 * 本函数不强制修改非法参数；参数合法性通常由调用者保证。
 * 在教学代码中，这样处理更直观。
 */
void mandelbrot_init(Mandelbrot *mb,
                     int max_iterations,
                     double escape_radius)
{
    if (mb == 0)
    {
        return;
    }

    mb->max_iterations = max_iterations;
    mb->escape_radius = escape_radius;
}

/**
 * @brief 计算点 c = cx + i cy 的逃逸迭代次数。
 *
 * Mandelbrot 集的经典迭代定义为：
 * @f[
 * z_0 = 0,\qquad z_{n+1} = z_n^2 + c
 * @f]
 * 其中
 * @f[
 * c = cx + i\,cy.
 * @f]
 *
 * 若在某一步首次满足
 * @f[
 * |z_n| > escape\_radius,
 * @f]
 * 则认为该点已经逃逸，并返回对应的迭代次数。
 *
 * 若在最大迭代次数内始终没有逃逸，则返回 max_iterations。
 *
 * 这里使用实部和虚部分别保存复数：
 * - 设 @f$ z_n = x_n + i y_n @f$
 * - 则
 *   @f[
 *   z_n^2 = (x_n^2 - y_n^2) + i(2x_n y_n)
 *   @f]
 * - 所以下一步迭代可写为
 *   @f[
 *   x_{n+1} = x_n^2 - y_n^2 + cx,
 *   @f]
 *   @f[
 *   y_{n+1} = 2x_n y_n + cy.
 *   @f]
 *
 * 这样的写法避免了依赖复数库，更适合 ANSI C 教学代码。
 *
 * @param mb 指向 Mandelbrot 参数结构的常量指针。
 * @param cx 复数参数 c 的实部。
 * @param cy 复数参数 c 的虚部。
 *
 * @return
 * - 若在最大迭代次数内逃逸，返回逃逸时的迭代次数；
 * - 若始终未逃逸，返回 mb->max_iterations。
 *
 * @note
 * 若 mb 为空指针，则返回 0。
 *
 * @note
 * 为了提高可读性，本实现直接比较
 * @f$ x^2 + y^2 @f$ 和 @f$ escape\_radius^2 @f$，
 * 避免每一步调用 sqrt()。
 */
int mandelbrot_escape_iterations(const Mandelbrot *mb,
                                 double cx,
                                 double cy)
{
    double x;
    double y;
    double x2;
    double y2;
    double escape_radius2;
    int iter;

    if (mb == 0)
    {
        return 0;
    }

    x = 0.0;
    y = 0.0;
    escape_radius2 = mb->escape_radius * mb->escape_radius;

    for (iter = 0; iter < mb->max_iterations; ++iter)
    {
        x2 = x * x;
        y2 = y * y;

        if (x2 + y2 > escape_radius2)
        {
            return iter;
        }

        y = 2.0 * x * y + cy;
        x = x2 - y2 + cx;
    }

    return mb->max_iterations;
}

/**
 * @brief 将迭代次数限制到合法范围内。
 *
 * 这是一个仅在本文件内部使用的辅助函数，
 * 用于防止颜色映射时迭代次数超出合理区间。
 *
 * @param value 待限制的迭代次数。
 * @param lower 允许的下界。
 * @param upper 允许的上界。
 *
 * @return 落在区间 [lower, upper] 内的整数值。
 */
static int clamp_int(int value, int lower, int upper)
{
    if (value < lower)
    {
        return lower;
    }
    if (value > upper)
    {
        return upper;
    }
    return value;
}

/**
 * @brief 根据逃逸迭代次数映射颜色。
 *
 * 本函数实现一个简单、直观、适合教学的颜色方案：
 *
 * - 若某点在最大迭代次数内未逃逸，则视为 Mandelbrot 集内部，
 *   返回黑色；
 * - 若某点逃逸较快，则给出较亮、较冷的颜色；
 * - 若某点逃逸较慢，则颜色逐渐向暖色过渡。
 *
 * 这里使用一个分段线性渐变，避免引入复杂配色公式，
 * 同时又比纯黑白图更能体现“逃逸快慢”的差异。
 *
 *
 * @param mb 指向 Mandelbrot 参数结构的常量指针。
 * @param iterations 某个点的逃逸迭代次数。
 *
 * @return 返回一个 RGB 颜色。
 *
 * @note
 * 若 iterations >= max_iterations，则返回黑色。
 *
 * @note
 * 若 mb 为空指针，返回黑色。
 */
RgbColor mandelbrot_color(const Mandelbrot *mb, int iterations)
{
    RgbColor color;
    int max_it;
    int level;   /* 0 ~ 1023 的颜色级别 */
    int seg;     /* 当前处于第几段 */
    int u;       /* 当前段内的局部位置 0 ~ 255 */

    /* 默认返回黑色：用于参数无效或“未逃逸”的点 */
    color.r = 0;
    color.g = 0;
    color.b = 0;

    if (mb == 0)
    {
        return color;
    }

    max_it = mb->max_iterations;
    if (max_it <= 0)
    {
        return color;
    }

    /*
     * 未逃逸：通常视为 Mandelbrot 集内部，画成黑色。
     */
    if (iterations >= max_it)
    {
        return color;
    }

    iterations = clamp_int(iterations, 0, max_it - 1);

    /*
     * 把迭代次数映射到 0 ~ 1023，共 4 段，每段 256 级。
     *
     * 与原来的 3 段 768 级相比：
     * 1. 段数更多；
     * 2. 颜色过渡更细；
     * 3. 边界附近的层次更明显。
     */
    level = (iterations * 1024) / max_it;
    if (level > 1023)
    {
        level = 1023;
    }

    seg = level / 256;
    u = level % 256;

    switch (seg)
    {
        case 0:
            /*
             * 第一段：深蓝 -> 蓝
             * 目的：
             * - 不从纯黑开始，避免外部快速逃逸点和内部黑色混在一起；
             * - 让“外部区域”一开始就与内部区分开。
             *
             * (0, 0, 64) -> (0, 0, 255)
             */
            color.r = 0;
            color.g = 0;
            color.b = (unsigned char)(64 + (u * 191) / 255);
            break;

        case 1:
            /*
             * 第二段：蓝 -> 青
             *
             * (0, 0, 255) -> (0, 255, 255)
             */
            color.r = 0;
            color.g = (unsigned char)u;
            color.b = 255;
            break;

        case 2:
            /*
             * 第三段：青 -> 黄
             *
             * (0, 255, 255) -> (255, 255, 0)
             *
             * 这一段通常会把靠近边界的区域提亮，
             * 使边界层次更容易看见。
             */
            color.r = (unsigned char)u;
            color.g = 255;
            color.b = (unsigned char)(255 - u);
            break;

        default:
            /*
             * 第四段：黄 -> 橙红
             *
             * (255, 255, 0) -> (255, 64, 0)
             *
             * 让最慢逃逸、最接近边界的点呈现暖色，
             * 视觉上更突出。
             */
            color.r = 255;
            color.g = (unsigned char)(255 - (u * 191) / 255);
            color.b = 0;
            break;
    }

    return color;
}