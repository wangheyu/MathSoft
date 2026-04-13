import os
import sys
import ctypes
import numpy as np
import matplotlib.pyplot as plt

# =========================
# 基本参数
# =========================
WIDTH, HEIGHT = 1024, 768
DPI = 100

INITIAL_VIEW_WIDTH = 3.5
INITIAL_CENTER_X = -0.5
INITIAL_CENTER_Y = 0.0

ZOOM_IN_FACTOR = 0.5
ZOOM_OUT_FACTOR = 2.0

center_x, center_y = INITIAL_CENTER_X, INITIAL_CENTER_Y
view_width = INITIAL_VIEW_WIDTH
view_height = view_width * HEIGHT / WIDTH

fig = None
ax = None
img_artist = None
lib = None


# =========================
# 加载 C 动态库
# =========================
def load_library():
    global lib

    if sys.platform.startswith("win"):
        libname = "mandelbrot_core.dll"
    elif sys.platform == "darwin":
        libname = "./libmandelbrot_core.dylib"
    else:
        libname = "./libmandelbrot_core.so"

    lib = ctypes.CDLL(libname)

    lib.mandelbrot_compute.argtypes = [
        ctypes.c_int,   # width
        ctypes.c_int,   # height
        ctypes.c_double,  # center_x
        ctypes.c_double,  # center_y
        ctypes.c_double,  # view_width
        ctypes.c_double,  # view_height
        ctypes.c_int,   # max_iter
        ctypes.POINTER(ctypes.c_double),  # smooth
        ctypes.POINTER(ctypes.c_uint8),   # inside_mask
    ]
    lib.mandelbrot_compute.restype = None


# =========================
# 自适应迭代次数
# =========================
def compute_max_iter(view_width):
    zoom = INITIAL_VIEW_WIDTH / view_width
    max_iter = int(100 + 40 * np.log10(max(zoom, 1.0)))
    return max(100, min(max_iter, 4000))


# =========================
# 调用 C 计算
# =========================
def mandelbrot_c(width, height, center_x, center_y, view_width, view_height, max_iter):
    smooth = np.zeros((height, width), dtype=np.float64)
    inside_mask = np.zeros((height, width), dtype=np.uint8)

    lib.mandelbrot_compute(
        width,
        height,
        center_x,
        center_y,
        view_width,
        view_height,
        max_iter,
        smooth.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
        inside_mask.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8)),
    )

    x_min = center_x - view_width / 2
    x_max = center_x + view_width / 2
    y_min = center_y - view_height / 2
    y_max = center_y + view_height / 2

    return smooth, inside_mask.astype(bool), (x_min, x_max, y_min, y_max)


# =========================
# 渲染
# =========================
def render():
    global img_artist, center_x, center_y, view_width, view_height

    view_height = view_width * HEIGHT / WIDTH
    max_iter = compute_max_iter(view_width)

    smooth, inside_mask, extent = mandelbrot_c(
        WIDTH, HEIGHT, center_x, center_y, view_width, view_height, max_iter
    )

    colored = smooth.copy()

    outside = ~inside_mask
    if np.any(outside):
        vmin = colored[outside].min()
        vmax = colored[outside].max()
        if vmax > vmin:
            colored[outside] = (colored[outside] - vmin) / (vmax - vmin)
        else:
            colored[outside] = 0.0

    colored[inside_mask] = 0.0

    if img_artist is None:
        img_artist = ax.imshow(
            colored,
            extent=extent,
            origin="lower",
            cmap="turbo",
            interpolation="bilinear",
        )
    else:
        img_artist.set_data(colored)
        img_artist.set_extent(extent)
        img_artist.set_cmap("turbo")

    zoom = INITIAL_VIEW_WIDTH / view_width
    ax.set_title(
        f"Mandelbrot Set\n"
        f"center=({center_x:.12f}, {center_y:.12f})  "
        f"width={view_width:.12e}  "
        f"zoom={zoom:.2f}x  "
        f"max_iter={max_iter}"
    )
    ax.set_xlabel("Re")
    ax.set_ylabel("Im")

    fig.canvas.draw_idle()


# =========================
# 鼠标事件
# =========================
def on_click(event):
    global center_x, center_y, view_width

    if event.inaxes != ax:
        return
    if event.xdata is None or event.ydata is None:
        return

    center_x = event.xdata
    center_y = event.ydata

    if event.button == 1:
        view_width *= ZOOM_IN_FACTOR
        render()
    elif event.button == 3:
        view_width *= ZOOM_OUT_FACTOR
        render()


# =========================
# 主程序
# =========================
def main():
    global fig, ax

    load_library()

    plt.rcParams["toolbar"] = "None"
    fig = plt.figure(figsize=(WIDTH / DPI, HEIGHT / DPI), dpi=DPI)
    ax = fig.add_subplot(111)

    render()
    fig.canvas.mpl_connect("button_press_event", on_click)
    plt.show()


if __name__ == "__main__":
    main()