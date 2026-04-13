#!/bin/bash

# ==========================================
# 用途：
#   调用 ./mandelbrot_app output.bmp center_x center_y half_width
#   生成一系列逐步放大的 BMP 图片，
#   然后用 ffmpeg 合成为视频。
# ==========================================

set -e

APP="./mandelbrot_app"
OUTDIR="frames"

# 选择一个 Mandelbrot 边界附近的经典放大中心
CENTER_X=-0.743643887037151
CENTER_Y=0.131825904205330

# 初始半宽
HALF_WIDTH=1.75

# 每一帧缩放倍率
# 小于 1 表示继续放大
ZOOM_FACTOR=0.97

# 总帧数
FRAMES=300

mkdir -p "$OUTDIR"

echo "开始生成 BMP 帧..."

for ((i=0; i<FRAMES; i++))
do
    FRAME_NAME=$(printf "%s/frame_%04d.bmp" "$OUTDIR" "$i")

    echo "生成第 $i 帧: $FRAME_NAME"
    echo "  center = ($CENTER_X, $CENTER_Y)"
    echo "  half_width = $HALF_WIDTH"

    "$APP" "$FRAME_NAME" "$CENTER_X" "$CENTER_Y" "$HALF_WIDTH"

    # 下一帧继续放大
    HALF_WIDTH=$(awk "BEGIN { printf \"%.15e\", $HALF_WIDTH * $ZOOM_FACTOR }")
done

echo "所有帧生成完成。"

echo "开始合成视频..."
ffmpeg -y -framerate 30 -i "$OUTDIR/frame_%04d.bmp" \
       -c:v libx264 -pix_fmt yuv420p mandelbrot_zoom.mp4

echo "完成：mandelbrot_zoom.mp4"