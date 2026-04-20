# -*- coding: utf-8 -*-
import json
import math
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from collections import deque


class TwoBodyApp:
    def __init__(self, root):
        self.root = root
        self.root.title("2D Two-Body Real-Time Simulation")

        self.canvas_width = 820
        self.canvas_height = 820

        self.running = False
        self.after_id = None

        self.t = 0.0
        self.r1 = [0.0, 0.0]
        self.r2 = [0.0, 0.0]
        self.v1 = [0.0, 0.0]
        self.v2 = [0.0, 0.0]
        self.m1 = 1.0
        self.m2 = 1.0
        self.G = 1.0
        self.dt = 0.01
        self.steps_per_frame = 4
        self.softening = 1e-4
        self.trail_len = 1500

        self.trail1 = deque(maxlen=self.trail_len)
        self.trail2 = deque(maxlen=self.trail_len)

        self.build_ui()
        self.draw_scene()

    def build_ui(self):
        main = ttk.Frame(self.root, padding=10)
        main.pack(fill="both", expand=True)

        left = ttk.Frame(main)
        left.pack(side="left", fill="y", padx=(0, 10))

        right = ttk.Frame(main)
        right.pack(side="left", fill="both", expand=True)

        ttk.Label(left, text="Input Parameters", font=("Arial", 12, "bold")).pack(anchor="w", pady=(0, 6))

        self.entries = {}

        fields = [
            ("G", "1.0"),
            ("m1", "1.0"),
            ("m2", "1.0"),
            ("x1", "-1.0"),
            ("y1", "0.0"),
            ("vx1", "0.0"),
            ("vy1", "0.5"),
            ("x2", "1.0"),
            ("y2", "0.0"),
            ("vx2", "0.0"),
            ("vy2", "-0.5"),
            ("dt", "0.01"),
            ("steps_per_frame", "4"),
        ]

        grid = ttk.Frame(left)
        grid.pack(fill="x")

        labels = {
            "G": "Gravitational constant G",
            "m1": "Mass of body 1 (m1)",
            "m2": "Mass of body 2 (m2)",
            "x1": "Initial x1",
            "y1": "Initial y1",
            "vx1": "Initial vx1",
            "vy1": "Initial vy1",
            "x2": "Initial x2",
            "y2": "Initial y2",
            "vx2": "Initial vx2",
            "vy2": "Initial vy2",
            "dt": "Time step dt",
            "steps_per_frame": "Steps per frame",
        }

        for i, (key, default) in enumerate(fields):
            ttk.Label(grid, text=labels[key], width=22).grid(row=i, column=0, sticky="w", pady=2)
            entry = ttk.Entry(grid, width=14)
            entry.insert(0, default)
            entry.grid(row=i, column=1, sticky="ew", pady=2)
            self.entries[key] = entry

        grid.columnconfigure(1, weight=1)

        ttk.Separator(left, orient="horizontal").pack(fill="x", pady=8)

        btns = ttk.Frame(left)
        btns.pack(fill="x")

        ttk.Button(btns, text="Start / Restart", command=self.start_simulation).pack(fill="x", pady=2)
        ttk.Button(btns, text="Pause / Resume", command=self.toggle_pause).pack(fill="x", pady=2)
        ttk.Button(btns, text="Reset View", command=self.reset_only_view).pack(fill="x", pady=2)
        ttk.Button(btns, text="Import Profile", command=self.import_profile).pack(fill="x", pady=2)

        tip = (
            "Notes:\n"
            "1. Use Import Profile to load a JSON initial-condition file.\n"
            "2. Smaller dt is more stable, but slower.\n"
            "3. Larger steps per frame makes simulation time advance faster.\n"
            "4. After importing, click Start / Restart to run."
        )
        ttk.Label(left, text=tip, justify="left", foreground="#444").pack(anchor="w", pady=(8, 0))

        self.canvas = tk.Canvas(
            right,
            width=self.canvas_width,
            height=self.canvas_height,
            bg="white",
            highlightthickness=1,
            highlightbackground="#BBBBBB",
        )
        self.canvas.pack(fill="both", expand=True)

        self.info_var = tk.StringVar(value="Simulation not started")
        info_bar = ttk.Label(right, textvariable=self.info_var, anchor="w")
        info_bar.pack(fill="x", pady=(6, 0))

    def set_entry(self, key, value):
        self.entries[key].delete(0, tk.END)
        self.entries[key].insert(0, str(value))

    def fill_entries_from_profile(self, profile):
        sim = profile["simulation"]
        body1 = profile["body1"]
        body2 = profile["body2"]

        self.set_entry("G", sim["G"])
        self.set_entry("dt", sim["dt"])
        self.set_entry("steps_per_frame", sim["steps_per_frame"])

        self.set_entry("m1", body1["mass"])
        self.set_entry("x1", body1["position"][0])
        self.set_entry("y1", body1["position"][1])
        self.set_entry("vx1", body1["velocity"][0])
        self.set_entry("vy1", body1["velocity"][1])

        self.set_entry("m2", body2["mass"])
        self.set_entry("x2", body2["position"][0])
        self.set_entry("y2", body2["position"][1])
        self.set_entry("vx2", body2["velocity"][0])
        self.set_entry("vy2", body2["velocity"][1])

    def validate_profile(self, profile):
        if not isinstance(profile, dict):
            raise ValueError("Profile root must be a JSON object.")

        required_top = ["profile_type", "simulation", "body1", "body2"]
        for key in required_top:
            if key not in profile:
                raise ValueError(f"Missing top-level field: {key}")

        if profile["profile_type"] != "two_body_2d":
            raise ValueError("profile_type must be 'two_body_2d'.")

        sim = profile["simulation"]
        body1 = profile["body1"]
        body2 = profile["body2"]

        for key in ["G", "dt", "steps_per_frame"]:
            if key not in sim:
                raise ValueError(f"Missing simulation field: {key}")

        for name, body in [("body1", body1), ("body2", body2)]:
            for key in ["mass", "position", "velocity"]:
                if key not in body:
                    raise ValueError(f"Missing {name} field: {key}")
            if len(body["position"]) != 2:
                raise ValueError(f"{name}.position must have length 2.")
            if len(body["velocity"]) != 2:
                raise ValueError(f"{name}.velocity must have length 2.")

    def import_profile(self):
        path = filedialog.askopenfilename(
            title="Import Profile",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if not path:
            return

        try:
            with open(path, "r", encoding="utf-8") as f:
                profile = json.load(f)

            self.validate_profile(profile)
            self.fill_entries_from_profile(profile)

            name = profile.get("name", "(unnamed)")
            self.info_var.set(f"Profile imported: {name}")
            messagebox.showinfo("Import Successful", f"Profile loaded successfully.\n\n{name}")

        except Exception as e:
            messagebox.showerror("Import Error", f"Failed to import profile.\n\nDetails: {e}")

    def parse_inputs(self):
        try:
            self.G = float(self.entries["G"].get())
            self.m1 = float(self.entries["m1"].get())
            self.m2 = float(self.entries["m2"].get())

            self.r1 = [float(self.entries["x1"].get()), float(self.entries["y1"].get())]
            self.v1 = [float(self.entries["vx1"].get()), float(self.entries["vy1"].get())]

            self.r2 = [float(self.entries["x2"].get()), float(self.entries["y2"].get())]
            self.v2 = [float(self.entries["vx2"].get()), float(self.entries["vy2"].get())]

            self.dt = float(self.entries["dt"].get())
            self.steps_per_frame = int(self.entries["steps_per_frame"].get())

            if self.m1 <= 0 or self.m2 <= 0:
                raise ValueError("Mass must be positive.")
            if self.G <= 0:
                raise ValueError("G must be positive.")
            if self.dt <= 0:
                raise ValueError("dt must be positive.")
            if self.steps_per_frame <= 0:
                raise ValueError("Steps per frame must be a positive integer.")

            dx = self.r2[0] - self.r1[0]
            dy = self.r2[1] - self.r1[1]
            if dx * dx + dy * dy < 1e-10:
                raise ValueError("The two bodies cannot start at exactly the same position.")

        except ValueError as e:
            messagebox.showerror("Input Error", f"Please check the input parameters.\n\nDetails: {e}")
            return False
        return True

    def start_simulation(self):
        if not self.parse_inputs():
            return

        self.stop_loop()
        self.running = True
        self.t = 0.0
        self.trail1 = deque(maxlen=self.trail_len)
        self.trail2 = deque(maxlen=self.trail_len)
        self.trail1.append(tuple(self.r1))
        self.trail2.append(tuple(self.r2))

        self.run_loop()

    def toggle_pause(self):
        self.running = not self.running
        if self.running:
            self.run_loop()
        else:
            self.stop_loop()

    def reset_only_view(self):
        self.stop_loop()
        self.running = False
        self.t = 0.0
        self.trail1.clear()
        self.trail2.clear()
        self.draw_scene()
        self.info_var.set("View reset. Parameters unchanged.")

    def stop_loop(self):
        if self.after_id is not None:
            self.root.after_cancel(self.after_id)
            self.after_id = None

    def acceleration(self, r1, r2):
        dx = r2[0] - r1[0]
        dy = r2[1] - r1[1]
        dist2 = dx * dx + dy * dy + self.softening * self.softening
        dist = math.sqrt(dist2)
        inv_r3 = 1.0 / (dist2 * dist)

        a1 = [self.G * self.m2 * dx * inv_r3, self.G * self.m2 * dy * inv_r3]
        a2 = [-self.G * self.m1 * dx * inv_r3, -self.G * self.m1 * dy * inv_r3]
        return a1, a2

    def step_verlet(self):
        a1_old, a2_old = self.acceleration(self.r1, self.r2)

        self.r1[0] += self.v1[0] * self.dt + 0.5 * a1_old[0] * self.dt * self.dt
        self.r1[1] += self.v1[1] * self.dt + 0.5 * a1_old[1] * self.dt * self.dt
        self.r2[0] += self.v2[0] * self.dt + 0.5 * a2_old[0] * self.dt * self.dt
        self.r2[1] += self.v2[1] * self.dt + 0.5 * a2_old[1] * self.dt * self.dt

        a1_new, a2_new = self.acceleration(self.r1, self.r2)

        self.v1[0] += 0.5 * (a1_old[0] + a1_new[0]) * self.dt
        self.v1[1] += 0.5 * (a1_old[1] + a1_new[1]) * self.dt
        self.v2[0] += 0.5 * (a2_old[0] + a2_new[0]) * self.dt
        self.v2[1] += 0.5 * (a2_old[1] + a2_new[1]) * self.dt

        self.t += self.dt
        self.trail1.append(tuple(self.r1))
        self.trail2.append(tuple(self.r2))

    def run_loop(self):
        if self.running:
            for _ in range(self.steps_per_frame):
                self.step_verlet()
            self.draw_scene()
            self.after_id = self.root.after(16, self.run_loop)

    def world_to_screen_builder(self):
        points = list(self.trail1) + list(self.trail2)
        points.extend([tuple(self.r1), tuple(self.r2)])

        if not points:
            points = [(0.0, 0.0), (1.0, 1.0)]

        xs = [p[0] for p in points]
        ys = [p[1] for p in points]

        min_x, max_x = min(xs), max(xs)
        min_y, max_y = min(ys), max(ys)

        cx = 0.5 * (min_x + max_x)
        cy = 0.5 * (min_y + max_y)

        span_x = max(max_x - min_x, 1e-6)
        span_y = max(max_y - min_y, 1e-6)

        span = max(span_x, span_y)
        span *= 1.25
        span = max(span, 2.0)

        scale = 0.9 * min(self.canvas_width, self.canvas_height) / span

        def convert(p):
            x = self.canvas_width / 2 + (p[0] - cx) * scale
            y = self.canvas_height / 2 - (p[1] - cy) * scale
            return x, y

        return convert, cx, cy, scale

    def draw_axes(self, convert):
        x0, y0 = convert((0.0, 0.0))
        self.canvas.create_line(0, y0, self.canvas_width, y0, fill="#E6E6E6")
        self.canvas.create_line(x0, 0, x0, self.canvas_height, fill="#E6E6E6")

    def draw_trail(self, trail, color, convert):
        if len(trail) < 2:
            return
        pts = []
        for p in trail:
            sx, sy = convert(p)
            pts.extend([sx, sy])
        self.canvas.create_line(*pts, fill=color, width=1.5, smooth=False)

    def draw_body(self, pos, color, label, convert):
        x, y = convert(tuple(pos))
        r = 5
        self.canvas.create_oval(x - r, y - r, x + r, y + r, fill=color, outline="")
        self.canvas.create_text(x + 12, y - 12, text=label, fill=color, font=("Arial", 10, "bold"))

    def compute_energy(self):
        v1sq = self.v1[0] ** 2 + self.v1[1] ** 2
        v2sq = self.v2[0] ** 2 + self.v2[1] ** 2

        dx = self.r2[0] - self.r1[0]
        dy = self.r2[1] - self.r1[1]
        dist = math.sqrt(dx * dx + dy * dy + self.softening * self.softening)

        kinetic = 0.5 * self.m1 * v1sq + 0.5 * self.m2 * v2sq
        potential = -self.G * self.m1 * self.m2 / dist
        return kinetic + potential, dist

    def draw_scene(self):
        self.canvas.delete("all")
        convert, cx, cy, scale = self.world_to_screen_builder()

        self.draw_axes(convert)
        self.draw_trail(self.trail1, "#4C78A8", convert)
        self.draw_trail(self.trail2, "#F58518", convert)
        self.draw_body(self.r1, "#4C78A8", "m1", convert)
        self.draw_body(self.r2, "#F58518", "m2", convert)

        energy, dist = self.compute_energy()

        self.canvas.create_text(
            12, 12,
            text="2D Two-Body Motion (Real Time)",
            anchor="nw",
            fill="#222222",
            font=("Arial", 12, "bold")
        )

        self.info_var.set(
            f"t = {self.t:.3f}    "
            f"|r2-r1| = {dist:.6f}    "
            f"E = {energy:.6f}    "
            f"view center = ({cx:.3f}, {cy:.3f})    "
            f"pixel scale = {scale:.2f}"
        )


def main():
    root = tk.Tk()
    app = TwoBodyApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
