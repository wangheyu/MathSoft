import tkinter as tk
from dataclasses import dataclass

import numpy as np
from PIL import Image, ImageTk


@dataclass
class Viewport:
    xmin: float
    xmax: float
    ymin: float
    ymax: float

    def width(self) -> float:
        return self.xmax - self.xmin

    def height(self) -> float:
        return self.ymax - self.ymin

    def pixel_to_complex(self, px: int, py: int, screen_w: int, screen_h: int) -> complex:
        x = self.xmin + (px / (screen_w - 1)) * self.width()
        y = self.ymax - (py / (screen_h - 1)) * self.height()
        return complex(x, y)

    def recenter_and_zoom(self, cx: float, cy: float, factor: float) -> "Viewport":
        new_w = self.width() * factor
        new_h = self.height() * factor
        return Viewport(
            xmin=cx - new_w / 2,
            xmax=cx + new_w / 2,
            ymin=cy - new_h / 2,
            ymax=cy + new_h / 2,
        )

    def translate(self, dx: float, dy: float) -> "Viewport":
        return Viewport(
            xmin=self.xmin + dx,
            xmax=self.xmax + dx,
            ymin=self.ymin + dy,
            ymax=self.ymax + dy,
        )


class MandelbrotRenderer:
    def __init__(self, width: int, height: int, max_iter: int = 200):
        self.width = width
        self.height = height
        self.max_iter = max_iter

    def compute_escape_counts(self, viewport: Viewport):
        xs = np.linspace(viewport.xmin, viewport.xmax, self.width, dtype=np.float64)
        ys = np.linspace(viewport.ymax, viewport.ymin, self.height, dtype=np.float64)
        X, Y = np.meshgrid(xs, ys)
        C = X + 1j * Y

        Z = np.zeros_like(C, dtype=np.complex128)
        counts = np.zeros(C.shape, dtype=np.int32)
        active = np.ones(C.shape, dtype=bool)

        for i in range(self.max_iter):
            Z[active] = Z[active] * Z[active] + C[active]
            escaped = np.abs(Z) > 2.0
            newly_escaped = escaped & active
            counts[newly_escaped] = i + 1
            active[newly_escaped] = False

            if not active.any():
                break

        counts[active] = self.max_iter
        return counts

    def colorize(self, counts):
        counts = counts.astype(np.float64)
        t = counts / self.max_iter

        rgb = np.zeros((self.height, self.width, 3), dtype=np.uint8)

        inside = counts >= self.max_iter
        outside = ~inside

        tt = t[outside]
        r = (9.0 * (1 - tt) * tt**3 * 255).clip(0, 255).astype(np.uint8)
        g = (15.0 * (1 - tt)**2 * tt**2 * 255).clip(0, 255).astype(np.uint8)
        b = (8.5 * (1 - tt)**3 * tt * 255).clip(0, 255).astype(np.uint8)

        rgb[..., 0][outside] = r
        rgb[..., 1][outside] = g
        rgb[..., 2][outside] = b

        rgb[..., 0][inside] = 0
        rgb[..., 1][inside] = 0
        rgb[..., 2][inside] = 0

        return rgb

    def render_rgb_array(self, viewport: Viewport):
        counts = self.compute_escape_counts(viewport)
        return self.colorize(counts)


class MandelbrotApp:
    def __init__(self, width: int = 1024, height: int = 768):
        self.width = width
        self.height = height

        self.root = tk.Tk()
        self.root.title("Mandelbrot Viewer (1024x768)")
        self.root.geometry(f"{width}x{height + 28}")
        self.root.resizable(False, False)

        self.canvas = tk.Canvas(self.root, width=width, height=height, highlightthickness=0)
        self.canvas.pack()

        self.status_var = tk.StringVar()
        self.status = tk.Label(self.root, textvariable=self.status_var, anchor="w", relief="sunken")
        self.status.pack(fill="x")

        self.renderer = MandelbrotRenderer(width, height, max_iter=200)

        self.initial_viewport = Viewport(
            xmin=-2.5,
            xmax=1.0,
            ymin=-1.3125,
            ymax=1.3125,
        )
        self.viewport = self._fit_aspect(self.initial_viewport)

        self.image = None
        self.drag_start = None
        self.drag_viewport = None

        self._bind_events()
        self.redraw()

    def _fit_aspect(self, viewport: Viewport) -> Viewport:
        screen_ratio = self.width / self.height
        view_ratio = viewport.width() / viewport.height()

        cx = (viewport.xmin + viewport.xmax) / 2
        cy = (viewport.ymin + viewport.ymax) / 2

        if view_ratio > screen_ratio:
            new_w = viewport.width()
            new_h = new_w / screen_ratio
        else:
            new_h = viewport.height()
            new_w = new_h * screen_ratio

        return Viewport(
            xmin=cx - new_w / 2,
            xmax=cx + new_w / 2,
            ymin=cy - new_h / 2,
            ymax=cy + new_h / 2,
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
        self.viewport = self.viewport.recenter_and_zoom(c.real, c.imag, factor=0.5)
        self.viewport = self._fit_aspect(self.viewport)
        self.redraw()

    def on_right_click(self, event):
        c = self.viewport.pixel_to_complex(event.x, event.y, self.width, self.height)
        self.viewport = self.viewport.recenter_and_zoom(c.real, c.imag, factor=2.0)
        self.viewport = self._fit_aspect(self.viewport)
        self.redraw()

    def on_middle_press(self, event):
        self.drag_start = (event.x, event.y)
        self.drag_viewport = self.viewport

    def on_middle_drag(self, event):
        if self.drag_start is None:
            return

        x0, y0 = self.drag_start
        dx_pixels = event.x - x0
        dy_pixels = event.y - y0

        dx = -dx_pixels * self.drag_viewport.width() / self.width
        dy = dy_pixels * self.drag_viewport.height() / self.height

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
        self.viewport = self._fit_aspect(self.initial_viewport)
        self.redraw()

    def on_decrease_iter(self, event):
        self.renderer.max_iter = max(20, self.renderer.max_iter - 50)
        self.redraw()

    def on_increase_iter(self, event):
        self.renderer.max_iter += 50
        self.redraw()

    def run(self):
        self.root.mainloop()


def main():
    app = MandelbrotApp(width=1024, height=768)
    app.run()


if __name__ == "__main__":
    main()