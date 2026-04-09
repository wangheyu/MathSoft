#include "mapping.h"

/**
 * @file mapping.c
 * @brief 像素坐标到复平面坐标的映射实现。
 *
 * 本文件实现 mapping.h 中声明的函数，用于完成以下任务：
 * - 初始化一个 Mapping 结构；
 * - 计算每个像素在复平面中对应的长度；
 * - 根据图像宽高比计算纵向显示范围；
 * - 将图像像素坐标 (px, py) 映射到复平面坐标。
 *
 * 该模块本身不负责 Mandelbrot 迭代，也不负责 BMP 写文件。
 * 它的职责仅限于“几何映射”：
 *
 * - 图像平面：以像素为单位；
 * - 复平面：以实数坐标表示；
 * - 本模块负责在这两者之间建立对应关系。
 *
 * 在整个程序中，主流程通常是：
 * - 遍历图像中的每个像素；
 * - 调用 mapping_pixel_to_complex() 得到复平面点；
 * - 将该点交给 Mandelbrot 模块计算逃逸次数；
 * - 再把颜色写入图像缓存；
 * - 最后调用 build_bmp() 输出 BMP 文件。
 */

/**
 * @brief 初始化 Mapping 结构体。
 *
 * 本函数把用户给定的图像尺寸和复平面观察窗口参数写入结构体。
 *
 * 其中：
 * - width, height 表示输出图像的尺寸；
 * - center_x, center_y 表示复平面观察窗口的中心；
 * - half_width 表示复平面横向显示范围的一半。
 *
 * 横向实际显示区间为
 * @f[
 * [center_x - half\_width,\; center_x + half\_width].
 * @f]
 *
 * 纵向显示范围不直接保存，而是在需要时根据图像宽高比动态计算。
 *
 * @param m
 *   指向待初始化 Mapping 结构体的指针。
 *
 * @param width
 *   图像宽度，单位为像素。
 *
 * @param height
 *   图像高度，单位为像素。
 *
 * @param center_x
 *   复平面观察窗口中心的实部坐标。
 *
 * @param center_y
 *   复平面观察窗口中心的虚部坐标。
 *
 * @param half_width
 *   复平面横向显示范围的一半。
 *
 * @note
 * 若 m 为空指针，则函数直接返回，不执行任何操作。
 *
 * @note
 * 本函数不强制修正非法参数。为了保持教学代码的直观性，
 * 参数合法性一般由调用者保证。
 */
void mapping_init(Mapping *m,
                  int width,
                  int height,
                  double center_x,
                  double center_y,
                  double half_width)
{
    if (m == 0)
    {
        return;
    }

    m->width = width;
    m->height = height;
    m->center_x = center_x;
    m->center_y = center_y;
    m->half_width = half_width;
}

/**
 * @brief 计算每个像素在复平面中对应的长度。
 *
 * 设图像宽度为 width，复平面横向半宽为 half_width，
 * 则整个复平面的横向显示宽度为
 * @f[
 * 2 \cdot half\_width.
 * @f]
 *
 * 因此，每个像素所对应的复平面长度为
 * @f[
 * pixel\_size = \frac{2 \cdot half\_width}{width}.
 * @f]
 *
 * 这个量可以理解为图像中水平方向相邻两个像素中心在复平面中的距离。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @return
 *   返回每个像素在复平面中的长度。
 *
 * @note
 * 若 m 为空指针，或 width 小于等于 0，则返回 0.0。
 */
double mapping_pixel_size(const Mapping *m)
{
    if (m == 0 || m->width <= 0)
    {
        return 0.0;
    }

    return 2.0 * m->half_width / (double)m->width;
}

/**
 * @brief 根据图像宽高比计算复平面窗口的纵向半高。
 *
 * 为了保证图像中的几何比例不失真，复平面中的显示区域
 * 应与图像本身具有相同的宽高比。
 *
 * 已知：
 * - 图像宽度为 width
 * - 图像高度为 height
 * - 复平面横向半宽为 half_width
 *
 * 则纵向半高取为
 * @f[
 * half\_height = half\_width \cdot \frac{height}{width}.
 * @f]
 *
 * 这样可以避免由于图像不是正方形而导致 Mandelbrot 图被拉伸。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @return
 *   返回纵向半高。
 *
 * @note
 * 若 m 为空指针，或 width 小于等于 0，则返回 0.0。
 */
double mapping_half_height(const Mapping *m)
{
    if (m == 0 || m->width <= 0)
    {
        return 0.0;
    }

    return m->half_width * (double)m->height / (double)m->width;
}

/**
 * @brief 将像素坐标映射为复平面坐标。
 *
 * 本函数是 mapping 模块最核心的功能。
 *
 * 设：
 * - 图像宽度为 width，高度为 height；
 * - 观察窗口中心为 (center_x, center_y)；
 * - 横向半宽为 half_width；
 * - 纵向半高为 half_height。
 *
 * 则复平面显示区域为
 * @f[
 * x \in [center_x - half\_width,\; center_x + half\_width],
 * @f]
 * @f[
 * y \in [center_y - half\_height,\; center_y + half\_height].
 * @f]
 *
 * 图像坐标系采用常见的屏幕坐标约定：
 * - 左上角像素为 (0, 0)；
 * - px 向右增大；
 * - py 向下增大。
 *
 * 复平面坐标系采用数学中的常见约定：
 * - x 向右增大；
 * - y 向上增大。
 *
 * 因此，y 方向在映射时需要“翻转”。
 *
 * 为了使采样更自然，本实现把每个像素的**中心点**
 * 映射到复平面中，而不是把像素左上角映射过去。
 * 因此使用：
 * @f[
 * px + 0.5,\qquad py + 0.5
 * @f]
 * 作为采样位置。
 *
 * 具体公式为：
 * @f[
 * x = (center_x - half\_width) + (px + 0.5)\cdot pixel\_size,
 * @f]
 * @f[
 * y = (center_y + half\_height) - (py + 0.5)\cdot pixel\_size.
 * @f]
 *
 * 上式中：
 * - 第一式表示从左边界向右走若干个像素；
 * - 第二式表示从上边界向下走若干个像素，
 *   但因为复平面 y 轴向上，所以需要做减法。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @param px
 *   像素横坐标，通常范围为 0 到 width - 1。
 *
 * @param py
 *   像素纵坐标，通常范围为 0 到 height - 1。
 *
 * @return
 *   返回一个 ComplexPoint，表示该像素对应的复平面点。
 *
 * @note
 * 若 m 为空指针，则返回 (0.0, 0.0)。
 *
 * @note
 * 本函数默认调用者传入的 px, py 在合法范围内。
 * 若超出范围，函数仍会按公式计算一个对应的复平面点，
 * 但该点通常不再对应实际图像中的有效像素。
 */
ComplexPoint mapping_pixel_to_complex(const Mapping *m, int px, int py)
{
    ComplexPoint p;
    double pixel_size;
    double half_height;
    double left;
    double top;

    p.x = 0.0;
    p.y = 0.0;

    if (m == 0)
    {
        return p;
    }

    pixel_size = mapping_pixel_size(m);
    half_height = mapping_half_height(m);

    left = m->center_x - m->half_width;
    top = m->center_y + half_height;

    p.x = left + ((double)px + 0.5) * pixel_size;
    p.y = top - ((double)py + 0.5) * pixel_size;

    return p;
}