import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['WenQuanYi Zen Hei']
plt.rcParams['axes.unicode_minus'] = False

# ============================================================
# 日地相对运动二维无量纲模型
#   r'' = -mu * r / |r|^3
# 其中 mu = 4*pi^2，对应单位圆轨道周期为 1 year
# ============================================================

MU = 4.0 * np.pi**2


def acceleration(r: np.ndarray, mu: float = MU) -> np.ndarray:
    """
    计算加速度 a(r) = -mu * r / |r|^3

    Parameters
    ----------
    r : np.ndarray
        位置向量, shape = (2,)
    mu : float
        引力参数

    Returns
    -------
    np.ndarray
        加速度向量, shape = (2,)
    """
    norm_r = np.linalg.norm(r)
    return -mu * r / norm_r**3


def energy(r: np.ndarray, v: np.ndarray, mu: float = MU) -> float:
    """
    计算相对运动机械能
        E = 1/2 |v|^2 - mu / |r|
    """
    return 0.5 * np.dot(v, v) - mu / np.linalg.norm(r)


def angular_momentum(r: np.ndarray, v: np.ndarray) -> float:
    """
    计算二维角动量
        L = x * vy - y * vx
    """
    return r[0] * v[1] - r[1] * v[0]


def euler_integrator(r0, v0, h, T, mu=MU):
    """
    显式 Euler 方法，对一阶系统
        r' = v
        v' = a(r)
    做积分

    Returns
    -------
    t : np.ndarray
    rs : np.ndarray, shape = (N+1, 2)
    vs : np.ndarray, shape = (N+1, 2)
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
    Velocity-Verlet 方法

    迭代格式：
        a_n = a(r_n)
        r_{n+1} = r_n + h v_n + (h^2/2) a_n
        a_{n+1} = a(r_{n+1})
        v_{n+1} = v_n + (h/2)(a_n + a_{n+1})

    Returns
    -------
    t : np.ndarray
    rs : np.ndarray, shape = (N+1, 2)
    vs : np.ndarray, shape = (N+1, 2)
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


def exact_solution_circle(t: np.ndarray):
    """
    圆轨道理论解：
        x(t) = cos(2*pi*t)
        y(t) = sin(2*pi*t)
    """
    x = np.cos(2.0 * np.pi * t)
    y = np.sin(2.0 * np.pi * t)
    return np.column_stack((x, y))


def compute_errors(rs, vs, mu=MU):
    """
    计算一周期后的：
      1. 终点位置误差：与理论终点 (1,0) 的距离
      2. 相对能量误差
      3. 相对角动量误差
    """
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


def run_one_case(method_name, integrator, h, T, r0, v0):
    """
    运行单组实验并返回误差指标
    """
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
    """
    打印实验结果表
    """
    print("=" * 90)
    print(f"{'方法':<18}{'步长 h':<12}{'位置误差':<20}{'相对能量误差':<20}{'相对角动量误差':<20}")
    print("=" * 90)
    for item in results:
        print(
            f"{item['method']:<18}"
            f"{item['h']:<12.1e}"
            f"{item['pos_err']:<20.8e}"
            f"{item['rel_E_err']:<20.8e}"
            f"{item['rel_L_err']:<20.8e}"
        )
    print("=" * 90)


def plot_trajectories(results, T):
    """
    绘制轨道对比图
    """
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    # 理论圆轨道
    tt = np.linspace(0.0, T, 2000)
    exact_r = exact_solution_circle(tt)

    # 按步长分组
    grouped = {}
    for item in results:
        h = item["h"]
        grouped.setdefault(h, []).append(item)

    for ax, (h, items) in zip(axes, sorted(grouped.items(), key=lambda x: x[0], reverse=True)):
        ax.plot(exact_r[:, 0], exact_r[:, 1], label="理论圆轨道", linewidth=2)

        for item in items:
            ax.plot(
                item["rs"][:, 0], item["rs"][:, 1],
                label=f"{item['method']}, h={item['h']:.0e}"
            )

        ax.scatter([0], [0], s=60, marker="o", label="原点/太阳")
        ax.set_aspect("equal")
        ax.set_xlabel("x")
        ax.set_ylabel("y")
        ax.set_title(f"圆轨道对比, h={h:.0e}")
        ax.legend()
        ax.grid(True)

    plt.tight_layout()
    plt.show()


def plot_error_time_history(result):
    """
    对单个实验，画出能量和角动量随时间的相对误差
    """
    t = result["t"]
    rs = result["rs"]
    vs = result["vs"]

    E0 = energy(rs[0], vs[0])
    L0 = angular_momentum(rs[0], vs[0])

    E_err = np.array([abs(energy(r, v) - E0) / abs(E0) for r, v in zip(rs, vs)])
    L_err = np.array([abs(angular_momentum(r, v) - L0) / abs(L0) for r, v in zip(rs, vs)])

    fig, axes = plt.subplots(1, 2, figsize=(12, 4))

    axes[0].plot(t, E_err)
    axes[0].set_title(f"{result['method']}, h={result['h']:.0e} 的能量相对误差")
    axes[0].set_xlabel("t")
    axes[0].set_ylabel("relative energy error")
    axes[0].grid(True)

    axes[1].plot(t, L_err)
    axes[1].set_title(f"{result['method']}, h={result['h']:.0e} 的角动量相对误差")
    axes[1].set_xlabel("t")
    axes[1].set_ylabel("relative angular momentum error")
    axes[1].grid(True)

    plt.tight_layout()
    plt.show()


def main():
    # --------------------------------------------------------
    # 圆轨道初值
    # --------------------------------------------------------
    r0 = np.array([1.0, 0.0], dtype=float)
    v0 = np.array([0.0, 2.0 * np.pi], dtype=float)

    # 模拟一整年（一个完整周期）
    T = 1.0

    step_sizes = [1e-2, 1e-3]

    results = []

    for h in step_sizes:
        results.append(run_one_case("Euler", euler_integrator, h, T, r0, v0))
        results.append(run_one_case("Velocity-Verlet", velocity_verlet_integrator, h, T, r0, v0))

    # 输出结果表
    print_results_table(results)

    # 绘制轨道图
    plot_trajectories(results, T)

    # 分别画误差随时间的变化（可按需注释掉）
    for item in results:
        plot_error_time_history(item)


if __name__ == "__main__":
    main()