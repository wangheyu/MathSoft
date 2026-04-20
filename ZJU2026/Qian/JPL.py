# JPL.py
#
# 作用：
#   1. 从 JPL Horizons 获取 Earth-Moon Barycenter 相对 Sun 的三维状态向量
#   2. 用三维二体模型 + Velocity-Verlet 数值传播
#   3. 在相同历元上与 JPL 参考状态比较
#   4. 输出位置/速度/日距的绝对误差与相对误差，并保存 CSV 和误差图
#
# 依赖：
#   pip install astroquery astropy pandas matplotlib numpy
#
# 单位：
#   - 数值传播内部统一使用 km, s
#   - Horizons vectors 原始返回 AU 和 AU/day，程序内自动换算

import os
import json
import numpy as np
import pandas as pd
import matplotlib

# 非交互后端，适合服务器/终端环境
matplotlib.use("Agg")
import matplotlib.pyplot as plt

from astroquery.jplhorizons import Horizons

# =========================
# 1. 常数与配置
# =========================

# JPL astrodynamic parameters (km^3 / s^2)
GM_SUN = 1.32712440041279419e11
GM_EARTH = 398600.435507
GM_MOON = 4902.800118
MU_SEMB = GM_SUN + GM_EARTH + GM_MOON

# 单位换算
AU_KM = 149597870.7
DAY_SEC = 86400.0

# Horizons 设置
TARGET = "3"         # Earth-Moon Barycenter
CENTER = "500@10"    # Sun body-center as origin

START_TIME = "2026-01-01"
STOP_TIME = "2027-01-01"
STEP_SIZE = "1d"     # Horizons 输出步长

# 数值传播内部步长（秒）
INTERNAL_DT_SEC = 3600.0

OUTDIR = "jpl_compare_output"
CSV_NAME = "sun_embary_compare.csv"
FIG_NAME = "sun_embary_compare_errors.png"
SUMMARY_NAME = "sun_embary_compare_summary.json"

# =========================
# 2. 三维二体模型
# =========================

def accel(r: np.ndarray, mu: float = MU_SEMB) -> np.ndarray:
    """三维二体加速度 a(r) = -mu * r / |r|^3"""
    nr = np.linalg.norm(r)
    return -mu * r / nr**3


def vv_step(r: np.ndarray, v: np.ndarray, dt: float, mu: float = MU_SEMB):
    """单步 Velocity-Verlet"""
    a0 = accel(r, mu)
    r1 = r + dt * v + 0.5 * dt * dt * a0
    a1 = accel(r1, mu)
    v1 = v + 0.5 * dt * (a0 + a1)
    return r1, v1


def propagate_segment_vv(
    r0: np.ndarray,
    v0: np.ndarray,
    dt_total: float,
    dt_internal: float = INTERNAL_DT_SEC,
    mu: float = MU_SEMB,
):
    """
    用固定步长 Velocity-Verlet 把状态从 t 推到 t + dt_total。
    最后一个子步允许小于 dt_internal，以保证精确落在比较时刻。
    """
    r = r0.copy()
    v = v0.copy()

    if dt_total < 0:
        raise ValueError("dt_total 必须非负")
    if dt_total == 0:
        return r, v

    n = int(dt_total // dt_internal)
    rem = dt_total - n * dt_internal

    for _ in range(n):
        r, v = vv_step(r, v, dt_internal, mu)

    if rem > 0:
        r, v = vv_step(r, v, rem, mu)

    return r, v

# =========================
# 3. 直接从 Horizons 获取结构化状态向量
# =========================

def fetch_horizons_vectors_structured(
    target: str = TARGET,
    center: str = CENTER,
    start_time: str = START_TIME,
    stop_time: str = STOP_TIME,
    step_size: str = STEP_SIZE,
) -> pd.DataFrame:
    """
    用 astroquery.jplhorizons 直接获取结构化 vectors 表。

    返回列：
      jd_tdb, calendar,
      x_jpl_km, y_jpl_km, z_jpl_km,
      vx_jpl_kms, vy_jpl_kms, vz_jpl_kms
    """
    obj = Horizons(
        id=target,
        location=center,
        epochs={
            "start": start_time,
            "stop": stop_time,
            "step": step_size,
        },
    )

    # 几何状态向量；参考平面取黄道面
    tbl = obj.vectors(refplane="ecliptic", aberrations="geometric")

    # astroquery 返回：
    #   x, y, z   : AU
    #   vx, vy, vz: AU/day
    df = pd.DataFrame({
        "jd_tdb": np.array(tbl["datetime_jd"], dtype=float),
        "calendar": np.array(tbl["datetime_str"]).astype(str),
        "x_jpl_km": np.array(tbl["x"], dtype=float) * AU_KM,
        "y_jpl_km": np.array(tbl["y"], dtype=float) * AU_KM,
        "z_jpl_km": np.array(tbl["z"], dtype=float) * AU_KM,
        "vx_jpl_kms": np.array(tbl["vx"], dtype=float) * AU_KM / DAY_SEC,
        "vy_jpl_kms": np.array(tbl["vy"], dtype=float) * AU_KM / DAY_SEC,
        "vz_jpl_kms": np.array(tbl["vz"], dtype=float) * AU_KM / DAY_SEC,
    })

    return df


def sanity_check_initial_state(df: pd.DataFrame):
    """
    检查初始状态量级是否合理。
    对 Sun–EM Bary 而言，应满足：
      |r0| ~ 1 AU ~ 1.5e8 km
      |v0| ~ 30 km/s
    """
    r0 = np.array([
        df.loc[0, "x_jpl_km"],
        df.loc[0, "y_jpl_km"],
        df.loc[0, "z_jpl_km"],
    ], dtype=float)
    v0 = np.array([
        df.loc[0, "vx_jpl_kms"],
        df.loc[0, "vy_jpl_kms"],
        df.loc[0, "vz_jpl_kms"],
    ], dtype=float)

    rnorm = np.linalg.norm(r0)
    vnorm = np.linalg.norm(v0)

    print(f"Initial |r0| = {rnorm:.6e} km")
    print(f"Initial |v0| = {vnorm:.6e} km/s")

    if not (1.0e8 <= rnorm <= 2.0e8):
        raise RuntimeError("初始位置量级异常：对 Sun–EM Bary 应为 1 AU 量级。")
    if not (10.0 <= vnorm <= 50.0):
        raise RuntimeError("初始速度量级异常：对 Sun–EM Bary 应为几十 km/s 量级。")

# =========================
# 4. 误差计算
# =========================

def add_numerical_solution_and_errors(
    df_ref: pd.DataFrame,
    mu: float = MU_SEMB,
    dt_internal: float = INTERNAL_DT_SEC,
) -> pd.DataFrame:
    """
    以 JPL 第一条状态作为初值，用三维二体模型数值传播到后续各参考历元。

    误差定义：
      eps_r_abs   = ||r_num - r_jpl||
      eps_r_rel   = ||r_num - r_jpl|| / ||r_jpl||
      eps_v_abs   = ||v_num - v_jpl||
      eps_v_rel   = ||v_num - v_jpl|| / ||v_jpl||
      eps_rho_abs = ||r_num| - |r_jpl||
      eps_rho_rel = eps_rho_abs / |r_jpl|
    """
    df = df_ref.copy().reset_index(drop=True)

    # 取第一条 JPL 状态作为初值
    r_num = np.array([
        df.loc[0, "x_jpl_km"],
        df.loc[0, "y_jpl_km"],
        df.loc[0, "z_jpl_km"],
    ], dtype=float)

    v_num = np.array([
        df.loc[0, "vx_jpl_kms"],
        df.loc[0, "vy_jpl_kms"],
        df.loc[0, "vz_jpl_kms"],
    ], dtype=float)

    jd_prev = float(df.loc[0, "jd_tdb"])

    x_num_list, y_num_list, z_num_list = [], [], []
    vx_num_list, vy_num_list, vz_num_list = [], [], []

    err_r_abs = []
    err_r_rel = []
    err_v_abs = []
    err_v_rel = []
    err_rho_abs = []
    err_rho_rel = []

    for i in range(len(df)):
        jd_now = float(df.loc[i, "jd_tdb"])

        if i > 0:
            dt_sec = (jd_now - jd_prev) * DAY_SEC
            r_num, v_num = propagate_segment_vv(r_num, v_num, dt_sec, dt_internal, mu)
            jd_prev = jd_now

        r_jpl = np.array([
            df.loc[i, "x_jpl_km"],
            df.loc[i, "y_jpl_km"],
            df.loc[i, "z_jpl_km"],
        ], dtype=float)

        v_jpl = np.array([
            df.loc[i, "vx_jpl_kms"],
            df.loc[i, "vy_jpl_kms"],
            df.loc[i, "vz_jpl_kms"],
        ], dtype=float)

        # 保存数值解
        x_num_list.append(r_num[0])
        y_num_list.append(r_num[1])
        z_num_list.append(r_num[2])
        vx_num_list.append(v_num[0])
        vy_num_list.append(v_num[1])
        vz_num_list.append(v_num[2])

        # 误差
        dr = r_num - r_jpl
        dv = v_num - v_jpl

        eps_r_abs = np.linalg.norm(dr)
        eps_v_abs = np.linalg.norm(dv)

        rho_num = np.linalg.norm(r_num)
        rho_jpl = np.linalg.norm(r_jpl)

        eps_rho_abs = abs(rho_num - rho_jpl)

        eps_r_rel = eps_r_abs / np.linalg.norm(r_jpl)
        eps_v_rel = eps_v_abs / np.linalg.norm(v_jpl)
        eps_rho_rel = eps_rho_abs / rho_jpl

        err_r_abs.append(eps_r_abs)
        err_r_rel.append(eps_r_rel)
        err_v_abs.append(eps_v_abs)
        err_v_rel.append(eps_v_rel)
        err_rho_abs.append(eps_rho_abs)
        err_rho_rel.append(eps_rho_rel)

    df["x_num_km"] = x_num_list
    df["y_num_km"] = y_num_list
    df["z_num_km"] = z_num_list
    df["vx_num_kms"] = vx_num_list
    df["vy_num_kms"] = vy_num_list
    df["vz_num_kms"] = vz_num_list

    df["err_r_abs_km"] = err_r_abs
    df["err_r_rel"] = err_r_rel
    df["err_v_abs_kms"] = err_v_abs
    df["err_v_rel"] = err_v_rel
    df["err_rho_abs_km"] = err_rho_abs
    df["err_rho_rel"] = err_rho_rel

    return df

# =========================
# 5. 摘要与输出
# =========================

def make_summary(df: pd.DataFrame):
    return {
        "max_err_r_abs_km": float(df["err_r_abs_km"].max()),
        "max_err_r_rel": float(df["err_r_rel"].max()),
        "max_err_v_abs_kms": float(df["err_v_abs_kms"].max()),
        "max_err_v_rel": float(df["err_v_rel"].max()),
        "max_err_rho_abs_km": float(df["err_rho_abs_km"].max()),
        "max_err_rho_rel": float(df["err_rho_rel"].max()),
        "final_err_r_abs_km": float(df["err_r_abs_km"].iloc[-1]),
        "final_err_r_rel": float(df["err_r_rel"].iloc[-1]),
        "final_err_v_abs_kms": float(df["err_v_abs_kms"].iloc[-1]),
        "final_err_v_rel": float(df["err_v_rel"].iloc[-1]),
        "final_err_rho_abs_km": float(df["err_rho_abs_km"].iloc[-1]),
        "final_err_rho_rel": float(df["err_rho_rel"].iloc[-1]),
    }


def save_outputs(
    df: pd.DataFrame,
    summary: dict,
    outdir: str = OUTDIR,
    csv_name: str = CSV_NAME,
    fig_name: str = FIG_NAME,
    summary_name: str = SUMMARY_NAME,
):
    os.makedirs(outdir, exist_ok=True)

    csv_path = os.path.join(outdir, csv_name)
    df.to_csv(csv_path, index=False, encoding="utf-8-sig")

    summary_path = os.path.join(outdir, summary_name)
    with open(summary_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)

    t_days = df["jd_tdb"] - df["jd_tdb"].iloc[0]

    fig, axes = plt.subplots(2, 2, figsize=(12, 8))

    axes[0, 0].plot(t_days, df["err_r_abs_km"])
    axes[0, 0].set_title("Position absolute error")
    axes[0, 0].set_xlabel("Days from start")
    axes[0, 0].set_ylabel("km")
    axes[0, 0].grid(True, alpha=0.3)

    axes[0, 1].plot(t_days, df["err_r_rel"])
    axes[0, 1].set_title("Position relative error")
    axes[0, 1].set_xlabel("Days from start")
    axes[0, 1].set_ylabel("dimensionless")
    axes[0, 1].grid(True, alpha=0.3)

    axes[1, 0].plot(t_days, df["err_v_abs_kms"])
    axes[1, 0].set_title("Velocity absolute error")
    axes[1, 0].set_xlabel("Days from start")
    axes[1, 0].set_ylabel("km/s")
    axes[1, 0].grid(True, alpha=0.3)

    axes[1, 1].plot(t_days, df["err_v_rel"])
    axes[1, 1].set_title("Velocity relative error")
    axes[1, 1].set_xlabel("Days from start")
    axes[1, 1].set_ylabel("dimensionless")
    axes[1, 1].grid(True, alpha=0.3)

    fig.tight_layout()
    fig_path = os.path.join(outdir, fig_name)
    fig.savefig(fig_path, dpi=220, bbox_inches="tight")
    plt.close(fig)

    return csv_path, fig_path, summary_path

# =========================
# 6. 主程序
# =========================

def main():
    print("Fetching structured JPL Horizons vectors...")
    df_ref = fetch_horizons_vectors_structured(
        target=TARGET,
        center=CENTER,
        start_time=START_TIME,
        stop_time=STOP_TIME,
        step_size=STEP_SIZE,
    )

    print(f"Reference rows: {len(df_ref)}")
    sanity_check_initial_state(df_ref)

    print("Propagating 3D two-body model and computing errors...")
    df_cmp = add_numerical_solution_and_errors(
        df_ref,
        mu=MU_SEMB,
        dt_internal=INTERNAL_DT_SEC,
    )

    summary = make_summary(df_cmp)

    print("\n=== Error summary ===")
    print(json.dumps(summary, indent=2, ensure_ascii=False))

    csv_path, fig_path, summary_path = save_outputs(
        df_cmp, summary, OUTDIR, CSV_NAME, FIG_NAME, SUMMARY_NAME
    )
    print(f"\nSaved CSV: {csv_path}")
    print(f"Saved figure: {fig_path}")
    print(f"Saved summary: {summary_path}")

    print("\nFirst 5 comparison rows:")
    print(df_cmp[[
        "calendar",
        "err_r_abs_km", "err_r_rel",
        "err_v_abs_kms", "err_v_rel",
        "err_rho_abs_km", "err_rho_rel"
    ]].head())


if __name__ == "__main__":
    main()