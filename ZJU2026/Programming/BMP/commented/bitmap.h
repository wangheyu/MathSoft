#ifndef BITMAP_H
#define BITMAP_H

/**
 * @file bitmap.h
 * @brief BMP 文件格式相关的数据结构与接口声明
 *
 * 本头文件定义：
 * - 与 BMP 文件格式对应的基本类型别名；
 * - BMP 文件头结构 BITMAPFILEHEADER；
 * - BMP 信息头结构 BITMAPINFOHEADER；
 * - 生成 24 位无压缩 BMP 文件的函数 build_bmp()。
 */

#include <stdint.h>

/**
 * @brief 1 字节无符号整数类型
 */
typedef uint8_t BYTE;

/**
 * @brief 2 字节无符号整数类型
 */
typedef uint16_t WORD;

/**
 * @brief 4 字节无符号整数类型
 */
typedef uint32_t DWORD;

/**
 * @brief 4 字节有符号整数类型
 */
typedef int32_t LONG;

/**
 * @brief BMP 文件头
 *
 * 这是 BMP 文件最前面的 14 个字节，用于描述：
 * - 文件类型；
 * - 整个文件大小；
 * - 像素数据起始位置。
 *
 * @note 使用 `__attribute__((packed))` 是为了防止编译器自动插入填充字节，
 *       从而保证结构体布局与 BMP 二进制格式严格一致。
 */
typedef struct __attribute__((packed)) tagBITMAPFILEHEADER
{
    WORD  bfType;      /**< 文件类型标记。对 BMP 应为字符 'B' 和 'M'，即 0x4D42。 */
    DWORD bfSize;      /**< 整个 BMP 文件的总大小，单位为字节。 */
    WORD  bfReserved1; /**< 保留字段，通常置为 0。 */
    WORD  bfReserved2; /**< 保留字段，通常置为 0。 */
    DWORD bfOffBits;   /**< 从文件开头到像素数据起始位置的偏移量，单位为字节。 */
} BITMAPFILEHEADER;

/**
 * @brief BMP 信息头
 *
 * 该结构通常占 40 字节，用于描述图像本身的属性，例如：
 * - 图像宽度与高度；
 * - 位深；
 * - 压缩方式；
 * - 像素数据大小；
 * - 分辨率信息。
 */
typedef struct __attribute__((packed)) tagBITMAPINFOHEADER
{
    DWORD biSize;          /**< 信息头结构本身的大小。对 BITMAPINFOHEADER 通常为 40。 */
    LONG  biWidth;         /**< 图像宽度，单位为像素。 */
    LONG  biHeight;        /**< 图像高度，单位为像素。正值表示自底向上存储，负值表示自顶向下存储。 */
    WORD  biPlanes;        /**< 颜色平面数。BMP 标准要求该值必须为 1。 */
    WORD  biBitCount;      /**< 每个像素的位数。本程序中固定为 24，即每像素 3 字节。 */
    DWORD biCompression;   /**< 压缩方式。0 表示 BI_RGB，即不压缩。 */
    DWORD biSizeImage;     /**< 像素数据区总大小，单位为字节，需计入每行补齐字节。 */
    LONG  biXPelsPerMeter; /**< 水平方向分辨率，单位为像素/米。 */
    LONG  biYPelsPerMeter; /**< 垂直方向分辨率，单位为像素/米。 */
    DWORD biClrUsed;       /**< 实际使用的调色板颜色数。24 位 BMP 通常为 0。 */
    DWORD biClrImportant;  /**< 重要颜色数。通常为 0，表示所有颜色都重要。 */
} BITMAPINFOHEADER;

/**
 * @brief 根据原始像素数组生成一个 24 位无压缩 BMP 文件
 *
 * 本函数将输入的像素数组写入 BMP 文件。输入图像需满足：
 * - 每个像素占 3 个字节；
 * - 字节顺序为 B、G、R；
 * - 内存排列顺序为从上到下、从左到右。
 *
 * @param file 输出 BMP 文件名，例如 `"test.bmp"`。
 * @param w 图像宽度，单位为像素。
 * @param h 图像高度，单位为像素。
 * @param image 原始像素数据首地址。
 *
 * @return 0 表示成功。
 * @return -1 表示参数不合法。
 * @return -2 表示输出文件无法打开。
 * @return -3 表示写文件过程中失败。
 */
int build_bmp(const char *file, int w, int h, const unsigned char *image);

#endif