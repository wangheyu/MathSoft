import numpy as np
import matplotlib.pyplot as plt

# 窗口大小
WIDTH, HEIGHT = 1024, 768
DPI = 100

# 初始视野
INITIAL_VIEW_WIDTH = 3.5
INITIAL_CENTER_X = -0.5
INITIAL_CENTER_Y = 0.0

# 缩放倍率
ZOOM_IN_FACTOR = 0.5
ZOOM_OUT_FACTOR = 2.0

# 当前视图参数
center_x, center_y = INITIAL_CENTER_X, INITIAL_CENTER_Y
view_width = INITIAL_VIEW_WIDTH
view_height = view_width * HEIGHT / WIDTH

fig = None
ax = None
img_artist = None


def compute_max_iter(view_width):
    """
    根据当前缩放程度自适应调整迭代次数。
    缩放越深，迭代次数越大。
    """
    zoom = INITIAL_VIEW_WIDTH / view_width
    max_iter = int(100 + 40 * np.log10(max(zoom, 1)))
    return max(100, min(max_iter, 2000))


def mandelbrot_smooth(width, height, center_x, center_y, view_width, view_height, max_iter):
    x_min = center_x - view_width / 2
    x_max = center_x + view_width / 2
    y_min = center_y - view_height / 2
    y_max = center_y + view_height / 2

    xs = np.linspace(x_min, x_max, width)
    ys = np.linspace(y_min, y_max, height)
    X, Y = np.meshgrid(xs, ys)
    C = X + 1j * Y
    Z = np.zeros_like(C)

    # 用于记录平滑值
    smooth = np.zeros(C.shape, dtype=np.float64)
    mask = np.ones(C.shape, dtype=bool)

    for i in range(max_iter):
        Z[mask] = Z[mask] * Z[mask] + C[mask]

        escaped = np.abs(Z) > 2.0
        newly_escaped = escaped & mask

        # 平滑着色公式
        if np.any(newly_escaped):
            abs_z = np.abs(Z[newly_escaped])
            smooth[newly_escaped] = i + 1 - np.log2(np.log2(abs_z))

        mask &= ~escaped
        if not mask.any():
            break

    # 集合内部点设为 0
    smooth[mask] = 0.0

    return smooth, mask, (x_min, x_max, y_min, y_max)


def render():
    global img_artist, center_x, center_y, view_width, view_height

    view_height = view_width * HEIGHT / WIDTH
    max_iter = compute_max_iter(view_width)

    smooth, inside_mask, extent = mandelbrot_smooth(
        WIDTH, HEIGHT, center_x, center_y, view_width, view_height, max_iter
    )

    # 归一化颜色值
    colored = smooth.copy()
    if np.any(~inside_mask):
        vmin = colored[~inside_mask].min()
        vmax = colored[~inside_mask].max()
        if vmax > vmin:
            colored[~inside_mask] = (colored[~inside_mask] - vmin) / (vmax - vmin)
        else:
            colored[~inside_mask] = 0.0

    # 集合内部保持黑色
    colored[inside_mask] = 0.0

    if img_artist is None:
        img_artist = ax.imshow(
            colored,
            extent=extent,
            origin="lower",
            cmap="turbo",   # 更丰富、更鲜艳的色系
            interpolation="bilinear"
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


def on_click(event):
    global center_x, center_y, view_width

    if event.inaxes != ax:
        return
    if event.xdata is None or event.ydata is None:
        return

    center_x = event.xdata
    center_y = event.ydata

    if event.button == 1:  # 左键放大
        view_width *= ZOOM_IN_FACTOR
        render()

    elif event.button == 3:  # 右键缩小
        view_width *= ZOOM_OUT_FACTOR
        render()


def main():
    global fig, ax

    plt.rcParams["toolbar"] = "None"

    fig = plt.figure(figsize=(WIDTH / DPI, HEIGHT / DPI), dpi=DPI)
    ax = fig.add_subplot(111)

    render()
    fig.canvas.mpl_connect("button_press_event", on_click)
    plt.show()


if __name__ == "__main__":
    main()