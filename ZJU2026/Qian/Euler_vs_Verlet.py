import os
import numpy as np
import matplotlib

# 使用非交互后端，避免 Qt / wayland 警告
matplotlib.use("Agg")

import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
from matplotlib.ticker import LogFormatterMathtext

# ============================================================
# 1. 全局绘图设置
# ============================================================

# 默认英文字体与数学字体
plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False
plt.rcParams["mathtext.fontset"] = "stix"
plt.rcParams["mathtext.default"] = "it"

plt.rcParams["figure.dpi"] = 140
plt.rcParams["savefig.dpi"] = 300
plt.rcParams["font.size"] = 11
plt.rcParams["axes.labelsize"] = 11
plt.rcParams["axes.titlesize"] = 12
plt.rcParams["legend.fontsize"] = 9

# 明确指定中文字体
CJK_FONT_NAME = "WenQuanYi Zen Hei"
CJK_FONT_PATH = fm.findfont(fm.FontProperties(family=CJK_FONT_NAME), fallback_to_default=False)
CJK_PROP = fm.FontProperties(fname=CJK_FONT_PATH)

# ============================================================
# 2. 无量纲模型参数
#   r'' = -mu * r / |r|^3
#   mu = 4*pi^2
# ============================================================

MU = 4.0 * np.pi**2
IMAGE_DIR = "images"


def ensure_output_dir(path: str):
    os.makedirs(path, exist_ok=True)


def acceleration(r: np.ndarray, mu: float = MU) -> np.ndarray:
    norm_r = np.linalg.norm(r)
    return -mu * r / norm_r**3


def energy(r: np.ndarray, v: np.ndarray, mu: float = MU) -> float:
    return 0.5 * np.dot(v, v) - mu / np.linalg.norm(r)


def angular_momentum(r: np.ndarray, v: np.ndarray) -> float:
    return r[0] * v[1] - r[1] * v[0]


def exact_solution_circle(t: np.ndarray) -> np.ndarray:
    x = np.cos(2.0 * np.pi * t)
    y = np.sin(2.0 * np.pi * t)
    return np.column_stack((x, y))


def h_to_mathtext(h: float) -> str:
    exponent = int(round(np.log10(h)))
    return rf"$h=10^{{{exponent}}}$"


# ============================================================
# 3. 数值方法
# ============================================================

def euler_integrator(r0, v0, h, T, mu=MU):
    """
    显式 Euler 方法：
        r_{n+1} = r_n + h v_n
        v_{n+1} = v_n + h a(r_n)
    """
    N = int(round(T / h))
    t = np.linspace(0.0, T, N + 1)

    rs = np.zeros((N + 1, 2), dtype=float)
    vs = np.zeros((N + 1, 2), dtype=float)

    rs[0] = r0
    vs[0] = v0

    for n in range(N):
        a_n = acceleration(rs[n], mu)
        rs[n + 1] = rs[n] + h * vs[n]
        vs[n + 1] = vs[n] + h * a_n

    return t, rs, vs


def velocity_verlet_integrator(r0, v0, h, T, mu=MU):
    """
    Velocity-Verlet 方法：
        a_n = a(r_n)
        r_{n+1} = r_n + h v_n + (h^2/2) a_n
        a_{n+1} = a(r_{n+1})
        v_{n+1} = v_n + (h/2)(a_n + a_{n+1})
    """
    N = int(round(T / h))
    t = np.linspace(0.0, T, N + 1)

    rs = np.zeros((N + 1, 2), dtype=float)
    vs = np.zeros((N + 1, 2), dtype=float)

    rs[0] = r0
    vs[0] = v0

    for n in range(N):
        a_n = acceleration(rs[n], mu)
        rs[n + 1] = rs[n] + h * vs[n] + 0.5 * h**2 * a_n
        a_np1 = acceleration(rs[n + 1], mu)
        vs[n + 1] = vs[n] + 0.5 * h * (a_n + a_np1)

    return t, rs, vs


# ============================================================
# 4. 误差计算
# ============================================================

def compute_errors(rs, vs, mu=MU):
    r0 = rs[0]
    v0 = vs[0]
    rN = rs[-1]
    vN = vs[-1]

    theoretical_final = np.array([1.0, 0.0])

    pos_err = np.linalg.norm(rN - theoretical_final)

    E0 = energy(r0, v0, mu)
    EN = energy(rN, vN, mu)
    rel_E_err = abs(EN - E0) / abs(E0)

    L0 = angular_momentum(r0, v0)
    LN = angular_momentum(rN, vN)
    rel_L_err = abs(LN - L0) / abs(L0)

    return pos_err, rel_E_err, rel_L_err


def run_case(method_name, integrator, r0, v0, h, T):
    t, rs, vs = integrator(r0, v0, h, T)
    pos_err, rel_E_err, rel_L_err = compute_errors(rs, vs)
    return {
        "method": method_name,
        "h": h,
        "t": t,
        "rs": rs,
        "vs": vs,
        "pos_err": pos_err,
        "rel_E_err": rel_E_err,
        "rel_L_err": rel_L_err,
    }


def print_results_table(results):
    print("=" * 106)
    print(f"{'方法':<18}{'步长 h':<12}{'位置误差':<24}{'相对能量误差':<24}{'相对角动量误差':<24}")
    print("=" * 106)
    for item in results:
        print(
            f"{item['method']:<18}"
            f"{item['h']:<12.1e}"
            f"{item['pos_err']:<24.8e}"
            f"{item['rel_E_err']:<24.8e}"
            f"{item['rel_L_err']:<24.8e}"
        )
    print("=" * 106)


# ============================================================
# 5. 绘图辅助函数
# ============================================================

def apply_cjk_to_legend(legend):
    """把图例文字全部设为中文字体。"""
    if legend is None:
        return
    for text in legend.get_texts():
        text.set_fontproperties(CJK_PROP)


def set_cjk_title(ax, title_text: str, math_text: str):
    """
    中文标题与数学公式分开渲染，避免字体冲突。
    """
    ax.set_title(title_text, fontproperties=CJK_PROP, pad=16)
    ax.text(
        0.5, 1.01, math_text,
        transform=ax.transAxes,
        ha="center", va="bottom",
        fontsize=12
    )


# ============================================================
# 6. 绘图函数
# ============================================================

def plot_circle_orbit_comparison(results, output_dir=IMAGE_DIR):
    ensure_output_dir(output_dir)

    fig, axes = plt.subplots(1, 2, figsize=(11.5, 5.2))

    tt = np.linspace(0.0, 1.0, 2000)
    exact_r = exact_solution_circle(tt)

    grouped = {}
    for item in results:
        grouped.setdefault(item["h"], []).append(item)

    ordered_h = sorted(grouped.keys(), reverse=True)

    for ax, h in zip(axes, ordered_h):
        items = grouped[h]

        ax.plot(
            exact_r[:, 0], exact_r[:, 1],
            color="black", linewidth=2.2, linestyle="-",
            label="理论圆轨道"
        )

        for item in items:
            if item["method"] == "Euler":
                ax.plot(
                    item["rs"][:, 0], item["rs"][:, 1],
                    color="tab:red", linewidth=1.6,
                    label="Euler 方法"
                )
            else:
                ax.plot(
                    item["rs"][:, 0], item["rs"][:, 1],
                    color="tab:blue", linewidth=1.8,
                    label="Velocity-Verlet 方法"
                )

        ax.scatter([0], [0], s=40, color="orange", zorder=5, label="原点/太阳")

        ax.set_aspect("equal", adjustable="box")
        ax.set_xlim(-1.35, 1.35)
        ax.set_ylim(-1.35, 1.35)

        ax.set_xlabel("横坐标 $x$", fontproperties=CJK_PROP)
        ax.set_ylabel("纵坐标 $y$", fontproperties=CJK_PROP)

        set_cjk_title(ax, "圆轨道数值对比", h_to_mathtext(h))

        ax.grid(True, linestyle="--", alpha=0.45)

        legend = ax.legend(loc="upper right", framealpha=0.95)
        apply_cjk_to_legend(legend)

    plt.tight_layout()
    save_path = os.path.join(output_dir, "circle_orbit_comparison.png")
    plt.savefig(save_path, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved: {save_path}")


def plot_conservation_errors(results, h_value, output_dir=IMAGE_DIR):
    ensure_output_dir(output_dir)

    selected = [item for item in results if abs(item["h"] - h_value) < 1e-15]
    if len(selected) != 2:
        raise ValueError(f"Cannot find two result sets for h={h_value}")

    fig, axes = plt.subplots(1, 2, figsize=(11.5, 4.8))

    for item in selected:
        t = item["t"]
        rs = item["rs"]
        vs = item["vs"]

        E0 = energy(rs[0], vs[0])
        L0 = angular_momentum(rs[0], vs[0])

        E_err = np.array([abs(energy(r, v) - E0) / abs(E0) for r, v in zip(rs, vs)])
        L_err = np.array([abs(angular_momentum(r, v) - L0) / abs(L0) for r, v in zip(rs, vs)])

        if item["method"] == "Euler":
            color = "tab:red"
            label = "Euler 方法"
        else:
            color = "tab:blue"
            label = "Velocity-Verlet 方法"

        axes[0].plot(t, E_err, linewidth=1.8, color=color, label=label)
        axes[1].plot(t, L_err, linewidth=1.8, color=color, label=label)

    for ax in axes:
        ax.set_yscale("log")
        ax.yaxis.set_major_formatter(LogFormatterMathtext())
        ax.set_xlabel(r"$t$")
        ax.grid(True, linestyle="--", alpha=0.45)
        legend = ax.legend(loc="best", framealpha=0.95)
        apply_cjk_to_legend(legend)

    axes[0].set_ylabel("相对能量误差", fontproperties=CJK_PROP)
    axes[1].set_ylabel("相对角动量误差", fontproperties=CJK_PROP)

    set_cjk_title(axes[0], "能量误差对比", h_to_mathtext(h_value))
    set_cjk_title(axes[1], "角动量误差对比", h_to_mathtext(h_value))

    plt.tight_layout()
    filename = f"conservation_errors_h{h_value:.0e}.png"
    save_path = os.path.join(output_dir, filename)
    plt.savefig(save_path, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved: {save_path}")


# ============================================================
# 7. 主程序
# ============================================================

def main():
    print(f"Using CJK font: {CJK_FONT_NAME}")

    r0 = np.array([1.0, 0.0], dtype=float)
    v0 = np.array([0.0, 2.0 * np.pi], dtype=float)

    T = 1.0
    step_sizes = [1e-2, 1e-3]

    results = []

    for h in step_sizes:
        results.append(run_case("Euler", euler_integrator, r0, v0, h, T))
        results.append(run_case("Velocity-Verlet", velocity_verlet_integrator, r0, v0, h, T))

    print_results_table(results)

    plot_circle_orbit_comparison(results, IMAGE_DIR)
    plot_conservation_errors(results, 1e-2, IMAGE_DIR)
    plot_conservation_errors(results, 1e-3, IMAGE_DIR)


if __name__ == "__main__":
    main()