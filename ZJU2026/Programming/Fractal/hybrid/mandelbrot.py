import ctypes
import os
import sys
import tkinter as tk
from dataclasses import dataclass

import numpy as np
from PIL import Image, ImageTk


@dataclass
class Viewport:
    """
    表示当前复平面中的观察区域。
    """
    xmin: float
    xmax: float
    ymin: float
    ymax: float

    def width(self) -> float:
        return self.xmax - self.xmin

    def height(self) -> float:
        return self.ymax - self.ymin

    def pixel_to_complex(self, px: int, py: int, screen_w: int, screen_h: int) -> complex:
        """
        将屏幕像素坐标映射到复平面点。
        """
        if screen_w <= 1:
            x = self.xmin
        else:
            x = self.xmin + (px / (screen_w - 1)) * self.width()

        if screen_h <= 1:
            y = self.ymax
        else:
            y = self.ymax - (py / (screen_h - 1)) * self.height()

        return complex(x, y)

    def recenter_and_zoom(self, cx: float, cy: float, factor: float) -> "Viewport":
        """
        以 (cx, cy) 为中心缩放。
        factor < 1 表示放大，factor > 1 表示缩小。
        """
        new_w = self.width() * factor
        new_h = self.height() * factor
        return Viewport(
            xmin=cx - new_w / 2.0,
            xmax=cx + new_w / 2.0,
            ymin=cy - new_h / 2.0,
            ymax=cy + new_h / 2.0,
        )

    def translate(self, dx: float, dy: float) -> "Viewport":
        """
        平移视野。
        """
        return Viewport(
            xmin=self.xmin + dx,
            xmax=self.xmax + dx,
            ymin=self.ymin + dy,
            ymax=self.ymax + dy,
        )


class MandelbrotCLibrary:
    """
    对 C 动态库进行简单封装。
    """

    def __init__(self, lib_path: str):
        self.lib = ctypes.CDLL(lib_path)

        self.lib.mandelbrot_escape_counts.argtypes = [
            ctypes.c_int,      # width
            ctypes.c_int,      # height
            ctypes.c_double,   # xmin
            ctypes.c_double,   # xmax
            ctypes.c_double,   # ymin
            ctypes.c_double,   # ymax
            ctypes.c_int,      # max_iter
            ctypes.POINTER(ctypes.c_int),  # output
        ]
        self.lib.mandelbrot_escape_counts.restype = None

    def compute_escape_counts(self, width: int, height: int,
                              xmin: float, xmax: float,
                              ymin: float, ymax: float,
                              max_iter: int) -> np.ndarray:
        """
        调用 C 函数，返回 shape = (height, width) 的 int32 数组。
        """
        counts = np.zeros(width * height, dtype=np.int32)

        self.lib.mandelbrot_escape_counts(
            width,
            height,
            xmin,
            xmax,
            ymin,
            ymax,
            max_iter,
            counts.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
        )

        return counts.reshape((height, width))


class MandelbrotRenderer:
    """
    使用 C 动态库计算逃逸次数，再由 Python 完成着色。
    """

    def __init__(self, width: int, height: int, lib_path: str, max_iter: int = 200):
        self.width = width
        self.height = height
        self.max_iter = max_iter
        self.c_lib = MandelbrotCLibrary(lib_path)

    def compute_escape_counts(self, viewport: Viewport) -> np.ndarray:
        return self.c_lib.compute_escape_counts(
            self.width,
            self.height,
            viewport.xmin,
            viewport.xmax,
            viewport.ymin,
            viewport.ymax,
            self.max_iter,
        )

    def colorize(self, counts: np.ndarray) -> np.ndarray:
        """
        根据逃逸次数着色。
        集内部显示为黑色，外部使用平滑色带。
        返回 shape=(height, width, 3) 的 uint8 RGB 数组。
        """
        counts_f = counts.astype(np.float64)
        t = counts_f / self.max_iter

        rgb = np.zeros((self.height, self.width, 3), dtype=np.uint8)

        inside = counts >= self.max_iter
        outside = ~inside

        tt = t[outside]

        r = (9.0 * (1.0 - tt) * tt**3 * 255.0).clip(0, 255).astype(np.uint8)
        g = (15.0 * (1.0 - tt)**2 * tt**2 * 255.0).clip(0, 255).astype(np.uint8)
        b = (8.5 * (1.0 - tt)**3 * tt * 255.0).clip(0, 255).astype(np.uint8)

        rgb[..., 0][outside] = r
        rgb[..., 1][outside] = g
        rgb[..., 2][outside] = b

        rgb[..., 0][inside] = 0
        rgb[..., 1][inside] = 0
        rgb[..., 2][inside] = 0

        return rgb

    def render_rgb_array(self, viewport: Viewport) -> np.ndarray:
        counts = self.compute_escape_counts(viewport)
        return self.colorize(counts)


class MandelbrotApp:
    """
    Tkinter 应用：负责窗口、交互和显示。
    """

    def __init__(self, width: int = 1024, height: int = 768, lib_path: str = "./mandelbrot_c.so"):
        self.width = width
        self.height = height

        self.root = tk.Tk()
        self.root.title("Mandelbrot Viewer (C accelerated)")
        self.root.geometry(f"{width}x{height + 28}")
        self.root.resizable(False, False)

        self.canvas = tk.Canvas(self.root, width=width, height=height, highlightthickness=0)
        self.canvas.pack()

        self.status_var = tk.StringVar()
        self.status = tk.Label(self.root, textvariable=self.status_var, anchor="w", relief="sunken")
        self.status.pack(fill="x")

        self.renderer = MandelbrotRenderer(width, height, lib_path=lib_path, max_iter=200)

        self.initial_viewport = self._fit_aspect(
            Viewport(
                xmin=-2.5,
                xmax=1.0,
                ymin=-1.3125,
                ymax=1.3125,
            )
        )
        self.viewport = self.initial_viewport

        self.image = None
        self.drag_start = None
        self.drag_viewport = None

        self._bind_events()
        self.redraw()

    def _fit_aspect(self, viewport: Viewport) -> Viewport:
        """
        调整视野，使其符合窗口宽高比，避免图像拉伸。
        """
        screen_ratio = self.width / self.height
        view_ratio = viewport.width() / viewport.height()

        cx = (viewport.xmin + viewport.xmax) / 2.0
        cy = (viewport.ymin + viewport.ymax) / 2.0

        if view_ratio > screen_ratio:
            new_w = viewport.width()
            new_h = new_w / screen_ratio
        else:
            new_h = viewport.height()
            new_w = new_h * screen_ratio

        return Viewport(
            xmin=cx - new_w / 2.0,
            xmax=cx + new_w / 2.0,
            ymin=cy - new_h / 2.0,
            ymax=cy + new_h / 2.0,
        )

    def _bind_events(self):
        self.canvas.bind("<Button-1>", self.on_left_click)
        self.canvas.bind("<Button-3>", self.on_right_click)

        self.canvas.bind("<ButtonPress-2>", self.on_middle_press)
        self.canvas.bind("<B2-Motion>", self.on_middle_drag)
        self.canvas.bind("<ButtonRelease-2>", self.on_middle_release)

        self.canvas.bind("<Shift-ButtonPress-1>", self.on_drag_press)
        self.canvas.bind("<Shift-B1-Motion>", self.on_drag_motion)
        self.canvas.bind("<Shift-ButtonRelease-1>", self.on_drag_release)

        self.root.bind("r", self.on_reset)
        self.root.bind("[", self.on_decrease_iter)
        self.root.bind("]", self.on_increase_iter)
        self.root.bind("<Escape>", lambda e: self.root.destroy())
        self.root.bind("q", lambda e: self.root.destroy())

    def update_status(self):
        self.status_var.set(
            f"region: x=[{self.viewport.xmin:.12f}, {self.viewport.xmax:.12f}]  "
            f"y=[{self.viewport.ymin:.12f}, {self.viewport.ymax:.12f}]  "
            f"max_iter={self.renderer.max_iter}"
        )

    def redraw(self):
        self.update_status()
        self.root.config(cursor="watch")
        self.root.update_idletasks()

        rgb = self.renderer.render_rgb_array(self.viewport)
        pil_image = Image.fromarray(rgb, mode="RGB")
        self.image = ImageTk.PhotoImage(pil_image)

        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor="nw", image=self.image)

        self.root.config(cursor="")

    def on_left_click(self, event):
        c = self.viewport.pixel_to_complex(event.x, event.y, self.width, self.height)
        self.viewport = self._fit_aspect(
            self.viewport.recenter_and_zoom(c.real, c.imag, factor=0.5)
        )
        self.redraw()

    def on_right_click(self, event):
        c = self.viewport.pixel_to_complex(event.x, event.y, self.width, self.height)
        self.viewport = self._fit_aspect(
            self.viewport.recenter_and_zoom(c.real, c.imag, factor=2.0)
        )
        self.redraw()

    def on_middle_press(self, event):
        self.drag_start = (event.x, event.y)
        self.drag_viewport = self.viewport

    def on_middle_drag(self, event):
        if self.drag_start is None or self.drag_viewport is None:
            return

        x0, y0 = self.drag_start
        dx_pixels = event.x - x0
        dy_pixels = event.y - y0

        dx = -dx_pixels * self.drag_viewport.width() / self.width
        dy =  dy_pixels * self.drag_viewport.height() / self.height

        self.viewport = self.drag_viewport.translate(dx, dy)
        self.redraw()

    def on_middle_release(self, event):
        self.drag_start = None
        self.drag_viewport = None

    def on_drag_press(self, event):
        self.on_middle_press(event)

    def on_drag_motion(self, event):
        self.on_middle_drag(event)

    def on_drag_release(self, event):
        self.on_middle_release(event)

    def on_reset(self, event):
        self.viewport = self.initial_viewport
        self.redraw()

    def on_decrease_iter(self, event):
        self.renderer.max_iter = max(20, self.renderer.max_iter - 50)
        self.redraw()

    def on_increase_iter(self, event):
        self.renderer.max_iter += 50
        self.redraw()

    def run(self):
        self.root.mainloop()


def default_library_name() -> str:
    if sys.platform.startswith("linux"):
        return "mandelbrot_c.so"
    if sys.platform == "darwin":
        return "mandelbrot_c.dylib"
    if os.name == "nt":
        return "mandelbrot_c.dll"
    return "mandelbrot_c.so"


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    lib_name = default_library_name()
    lib_path = os.path.join(script_dir, lib_name)

    if not os.path.exists(lib_path):
        raise FileNotFoundError(
            f"找不到动态库文件: {lib_path}\n"
            f"请先编译 C 文件生成 {lib_name}"
        )

    app = MandelbrotApp(width=1024, height=768, lib_path=lib_path)
    app.run()


if __name__ == "__main__":
    main()