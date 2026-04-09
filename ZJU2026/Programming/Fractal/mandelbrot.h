#ifndef MANDELBROT_H
#define MANDELBROT_H

/**
 * @file mandelbrot.h
 * @brief Mandelbrot 集的迭代计算与颜色映射接口。
 *
 * 本头文件定义了与 Mandelbrot 集绘制相关的核心数据结构和函数：
 *
 * 1. Mandelbrot 计算参数的保存；
 * 2. 对复平面点 c = cx + i cy 进行逃逸迭代次数计算；
 * 3. 根据迭代次数将像素映射为 RGB 颜色。
 *
 * 设计目标：
 * - 保持 ANSI C 风格；
 * - 接口简单、职责清晰；
 * - 便于教学和阅读；
 * - 不依赖 C++ 类与继承机制。
 *
 * 在典型用法中，主程序会：
 * - 先创建并初始化一个 Mandelbrot 结构；
 * - 对每个像素对应的复平面点调用 mandelbrot_escape_iterations()；
 * - 再调用 mandelbrot_color() 得到颜色；
 * - 最后把颜色写入 BMP 图像缓存。
 */

/**
 * @struct RgbColor
 * @brief 表示一个 RGB 颜色。
 *
 * 该结构体保存一个像素的三原色分量：
 * - r: 红色分量
 * - g: 绿色分量
 * - b: 蓝色分量
 *
 * 每个分量都使用一个 unsigned char，取值范围通常为 0 到 255。
 *
 * 注意：
 * 某些 BMP 写入函数在内存中要求按 B、G、R 的顺序存放字节，
 * 但这里的数据结构仍按更直观的 r、g、b 命名。
 * 因此在写入位图缓冲区时，主程序或实现文件可以按需要调整写入顺序。
 */
typedef struct
{
    unsigned char r; /**< 红色分量，范围通常为 0~255 */
    unsigned char g; /**< 绿色分量，范围通常为 0~255 */
    unsigned char b; /**< 蓝色分量，范围通常为 0~255 */
} RgbColor;

/**
 * @struct Mandelbrot
 * @brief 保存 Mandelbrot 集计算所需的参数。
 *
 * 本结构体只保存“整张图像渲染过程中共享的参数”，而不保存某个像素点
 * 的临时迭代状态。这样设计的优点是：
 *
 * - 结构简单；
 * - 易于理解；
 * - 更符合 C 语言“数据 + 函数”的组织方式；
 * - 避免为每个像素重复创建复杂对象。
 *
 * 字段含义：
 * - max_iterations: 最大迭代次数
 * - escape_radius: 逃逸半径
 *
 * 对于 Mandelbrot 集的经典判定，通常取 escape_radius = 2.0。
 */
typedef struct
{
    int max_iterations;   /**< 最大迭代次数 */
    double escape_radius; /**< 逃逸半径，通常取 2.0 */
} Mandelbrot;

/**
 * @brief 初始化 Mandelbrot 参数结构。
 *
 * 该函数用于给一个 Mandelbrot 结构体赋初值。
 *
 * 典型示例：
 * @code
 * Mandelbrot mb;
 * mandelbrot_init(&mb, 500, 2.0);
 * @endcode
 *
 * @param mb
 *   指向待初始化的 Mandelbrot 结构体的指针。
 *
 * @param max_iterations
 *   最大迭代次数。
 *   该值越大，图像边界细节通常越丰富，但计算时间也会增加。
 *
 * @param escape_radius
 *   逃逸半径。
 *   对于经典 Mandelbrot 集，通常使用 2.0。
 */
void mandelbrot_init(Mandelbrot *mb,
                     int max_iterations,
                     double escape_radius);

/**
 * @brief 计算点 c = cx + i cy 的逃逸迭代次数。
 *
 * Mandelbrot 集的经典迭代定义为：
 * @f[
 * z_0 = 0,\qquad z_{n+1} = z_n^2 + c
 * @f]
 * 其中：
 * @f[
 * c = cx + i\,cy
 * @f]
 *
 * 本函数从 @f$ z_0 = 0 @f$ 开始迭代，并在每一步检测：
 * @f[
 * |z_n| > R
 * @f]
 * 其中 @f$ R @f$ 为 escape_radius。
 *
 * 若某一步首次满足逃逸条件，则返回该步对应的迭代次数；
 * 若在最大迭代次数内始终没有逃逸，则返回 max_iterations。
 *
 * 这样设计有两个好处：
 * - 逻辑清晰，便于教学；
 * - 返回值既可用于集合内外判断，也可直接用于颜色映射。
 *
 * 一般约定：
 * - 返回值 < max_iterations ：说明该点较快逃逸，通常在集合外部；
 * - 返回值 == max_iterations：说明在给定上限内未逃逸，通常将其视为集合内部或边界附近。
 *
 * @param mb
 *   指向 Mandelbrot 参数结构的常量指针。
 *
 * @param cx
 *   复数参数 c 的实部。
 *
 * @param cy
 *   复数参数 c 的虚部。
 *
 * @return
 *   返回逃逸迭代次数。
 *   若未在规定步数内逃逸，则返回 mb->max_iterations。
 */
int mandelbrot_escape_iterations(const Mandelbrot *mb,
                                 double cx,
                                 double cy);

/**
 * @brief 根据迭代次数生成一个 RGB 颜色。
 *
 * 该函数把 escape_iterations 的结果映射为颜色，供图像输出使用。
 *
 * 最基本的设计思路是：
 * - 如果某点在最大迭代次数内没有逃逸，则把它画成黑色；
 * - 如果某点较早逃逸，则根据“逃逸的快慢”赋予不同颜色。
 *
 * 颜色映射函数本身并不改变 Mandelbrot 数学模型，
 * 它只是把数值结果转换为可视化结果。
 *
 * 在教学演示中，可以采用较简单的颜色规则，例如：
 * - 内部点：黑色；
 * - 外部点：按迭代次数比例生成蓝色、青色、黄色等渐变。
 *
 * 由于返回值是 RgbColor，因此主程序只需把结果写入像素缓冲区即可。
 *
 * @param mb
 *   指向 Mandelbrot 参数结构的常量指针。
 *   颜色映射函数可能需要用到 max_iterations，以便根据比例生成颜色。
 *
 * @param iterations
 *   某个像素对应点的逃逸迭代次数。
 *   该值通常由 mandelbrot_escape_iterations() 返回。
 *
 * @return
 *   返回一个 RGB 颜色结构。
 */
RgbColor mandelbrot_color(const Mandelbrot *mb, int iterations);

#endif /* MANDELBROT_H */