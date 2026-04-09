#ifndef MAPPING_H
#define MAPPING_H

/**
 * @file mapping.h
 * @brief 像素坐标到复平面坐标的映射接口。
 *
 * 本头文件定义了绘制 Mandelbrot 图像时所需的“坐标映射”模块。
 *
 * 在图像渲染过程中，程序通常会遍历每一个像素点 (px, py)，
 * 然后把它转换为复平面上的一个点
 * @f[
 * c = x + iy .
 * @f]
 * 随后再把这个复数点交给 Mandelbrot 迭代模块计算逃逸次数。
 *
 * 因此，本模块的职责是：
 * - 保存图像宽度和高度；
 * - 保存复平面观察窗口的中心位置；
 * - 保存复平面窗口的横向半宽；
 * - 计算每个像素对应的复平面长度；
 * - 实现“像素坐标 -> 复平面坐标”的转换。
 *
 * 设计原则：
 * - 只处理几何映射，不参与 Mandelbrot 迭代；
 * - 使用简单的结构体和函数，适合 ANSI C 教学代码；
 * - 接口清晰，便于和 mandelbrot.h 配合使用。
 */

/**
 * @struct ComplexPoint
 * @brief 表示复平面中的一个点。
 *
 * 该结构体并不直接使用 C99 的 complex 类型，
 * 而是使用两个 double 成员来分别保存实部和虚部。
 *
 * 这样做有两个好处：
 * - 更直观，适合教学；
 * - 避免额外依赖，便于在纯 ANSI C 风格下使用。
 *
 * 数学上，它表示复数
 * @f[
 * z = x + iy .
 * @f]
 */
typedef struct
{
    double x; /**< 复数的实部，或平面点的横坐标 */
    double y; /**< 复数的虚部，或平面点的纵坐标 */
} ComplexPoint;

/**
 * @struct Mapping
 * @brief 保存图像像素平面到复平面的映射参数。
 *
 * 该结构体描述了一个“观察窗口”，用于决定：
 * - 输出图像有多大；
 * - 复平面上观察区域的中心在哪里；
 * - 横向显示范围有多宽。
 *
 * 本结构中的各字段含义如下：
 *
 * - width:
 *   输出图像的宽度（单位：像素）
 *
 * - height:
 *   输出图像的高度（单位：像素）
 *
 * - center_x, center_y:
 *   复平面观察窗口的中心坐标
 *
 * - half_width:
 *   复平面窗口横向范围的一半
 *
 * 因此，横向显示区间为
 * @f[
 * [center_x - half_width,\; center_x + half_width].
 * @f]
 *
 * 为了保证图像比例不失真，纵向半高不直接存储，
 * 而是由宽高比自动计算：
 * @f[
 * half\_height = half\_width \cdot \frac{height}{width}.
 * @f]
 *
 * 这种设计有助于保持复平面中的几何比例，
 * 避免图像被拉伸或压缩。
 */
typedef struct
{
    int width;        /**< 图像宽度，单位为像素 */
    int height;       /**< 图像高度，单位为像素 */
    double center_x;  /**< 观察窗口中心的实部坐标 */
    double center_y;  /**< 观察窗口中心的虚部坐标 */
    double half_width;/**< 观察窗口横向范围的一半 */
} Mapping;

/**
 * @brief 初始化 Mapping 结构体。
 *
 * 该函数为一个 Mapping 变量设置初始值，使其能够描述
 * 一个完整的“像素平面到复平面”的映射关系。
 *
 * 典型用法如下：
 * @code
 * Mapping map;
 * mapping_init(&map, 1920, 1080, -0.5, 0.0, 1.5);
 * @endcode
 *
 * 上面的调用表示：
 * - 图像大小为 1920 x 1080；
 * - 复平面窗口中心为 (-0.5, 0.0)；
 * - 横向半宽为 1.5；
 * - 因而横向总宽度为 3.0。
 *
 * @param m
 *   指向待初始化的 Mapping 结构体。
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
 *   复平面窗口横向范围的一半。
 */
void mapping_init(Mapping *m,
                  int width,
                  int height,
                  double center_x,
                  double center_y,
                  double half_width);

/**
 * @brief 返回每个像素对应的复平面长度。
 *
 * 设复平面观察窗口的横向半宽为 half_width，
 * 图像宽度为 width，则横向总宽度为
 * @f[
 * 2 \cdot half\_width .
 * @f]
 *
 * 因此，每一个像素在复平面中对应的长度为
 * @f[
 * pixel\_size = \frac{2 \cdot half\_width}{width}.
 * @f]
 *
 * 该值可理解为：
 * - 在复平面中，图像横向相邻两个像素中心之间的距离；
 * - 也是绘制时的基本采样步长。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @return
 *   返回每个像素对应的复平面长度。
 */
double mapping_pixel_size(const Mapping *m);

/**
 * @brief 根据图像宽高比，计算复平面窗口的纵向半高。
 *
 * 如果图像宽高比为
 * @f[
 * \frac{width}{height},
 * @f]
 * 为了保持图像不变形，复平面窗口也应保持相同的比例。
 *
 * 已知横向半宽为 half_width，则纵向半高应取为
 * @f[
 * half\_height = half\_width \cdot \frac{height}{width}.
 * @f]
 *
 * 这样，复平面的显示区域就不会因为图像不是正方形而被拉伸。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @return
 *   返回复平面观察窗口的纵向半高。
 */
double mapping_half_height(const Mapping *m);

/**
 * @brief 将像素坐标 (px, py) 映射为复平面坐标。
 *
 * 这是本模块最核心的函数。
 *
 * 设：
 * - 图像宽度为 width，高度为 height；
 * - 复平面窗口中心为 (center_x, center_y)；
 * - 横向半宽为 half_width；
 * - 纵向半高为 half_height。
 *
 * 则复平面显示区域为：
 * @f[
 * x \in [center_x - half_width,\; center_x + half_width],
 * @f]
 * @f[
 * y \in [center_y - half_height,\; center_y + half_height].
 * @f]
 *
 * 图像坐标系通常约定：
 * - 左上角像素为 (0, 0)；
 * - px 向右增大；
 * - py 向下增大。
 *
 * 而复平面中通常约定：
 * - x 向右增大；
 * - y 向上增大。
 *
 * 因此，在映射时需要注意 y 方向是相反的。
 *
 * 一个常见的映射方式是把像素中心映射到复平面中对应位置，
 * 从而得到更自然的采样点。实现时通常会使用像素中心偏移量
 * （例如 0.5 个像素）来进行转换。
 *
 * 返回值中的：
 * - result.x 表示复数实部；
 * - result.y 表示复数虚部。
 *
 * @param m
 *   指向 Mapping 结构体的常量指针。
 *
 * @param px
 *   像素的横坐标，范围通常为 0 到 width - 1。
 *
 * @param py
 *   像素的纵坐标，范围通常为 0 到 height - 1。
 *
 * @return
 *   返回一个 ComplexPoint，表示该像素对应的复平面点。
 */
ComplexPoint mapping_pixel_to_complex(const Mapping *m, int px, int py);

#endif /* MAPPING_H */