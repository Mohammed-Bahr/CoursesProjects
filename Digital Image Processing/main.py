#!/usr/bin/env python3
"""
=============================================================================
 Task : Use the image histogram to DEDUCE the parameters for contrast
        stretching, then APPLY the stretch on the corresponding image.
        Hint from the assignment: use cv2.LUT  (point ops are done fast
        with a Lookup-Table, see notes 00.Foundations&Enhancement of DIP.md)
=============================================================================

Theory (from the course notes, sections 2.4 (4) and 2.5):

  1. Contrast stretching = linearly map a narrow range [r1, r2] of the
     input intensities onto the FULL output range [0, 255]:

             s = ( (s2 - s1) / (r2 - r1) ) * (r - r1) + s1

     with the control points   (r1, s1) = (X_min , 0)
                               (r2, s2) = (X_max , 255)

  2. This is the SPECIAL CASE of the piecewise-linear transform whose
     three segments are:

         s = 0                          for  r <= r1      (crush blacks)
         s = 255/(r2-r1) * (r - r1)     for  r1 < r <= r2 (steep, slope > 1
                                                           -> CONTRAST BOOST)
         s = 255                        for  r >  r2      (clip whites)

  3. Because a point operation depends ONLY on the intensity value r,
     we can precompute all 256 outputs ONCE into a Lookup Table and apply
     it in a single pass with  cv2.LUT(img, lut)   <- very fast.

  4. Deducing r1, r2 FROM THE HISTOGRAM (the core of this task):
       - "minmax"      : r1 = smallest intensity in the image,
                         r2 = largest  intensity in the image.
                         -> exact formula from the notes, BUT a single
                            outlier pixel ruins the range (notes warning).
       - "percentile"  : r1 = intensity at the p_low percentile  (default 2%),
                         r2 = intensity at the p_high percentile (default 98%).
                         -> the notes RECOMMEND this real-world version.
       - "cdf"         : walk the cumulative histogram and take the first
                         level where CDF >= p_low / p_high.
                         -> reading the parameters straight off the CDF.

Usage:
    python3 contrast_stretching.py                        # auto low-contrast sample
    python3 contrast_stretching.py path/to/image.jpg       # your own image
    python3 contrast_stretching.py img.png --method minmax --low 2 --high 98
=============================================================================
"""

from __future__ import annotations

import argparse
from pathlib import Path

import cv2
import numpy as np
import matplotlib

matplotlib.use("Agg")  # headless backend so the script works over SSH/terminals
import matplotlib.pyplot as plt

# Number of intensity levels for 8-bit images: intensities run 0 .. L-1
L = 256
MAX_LEVEL = L - 1  # 255


# ---------------------------------------------------------------------------
# 0) Sample image: create a synthetic LOW-CONTRAST image so the script can be
#    demonstrated even without an external photo (deterministic, offline).
#    Real low-contrast photos (like the "pollen" slide image from the lecture)
#    behave exactly the same way.
# ---------------------------------------------------------------------------
def make_low_contrast_sample(out_path: Path) -> np.ndarray:
    """Build a scene whose intensities only live inside a narrow band."""
    rng = np.random.default_rng(42)  # fixed seed -> reproducible image
    h, w = 360, 480

    yy, xx = np.mgrid[0:h, 0:w]

    # A smooth "landscape": gradient + two bright blobs + some texture
    scene = 90 + 60 * (yy / h)  # gradient
    scene += 120 * np.exp(-(((xx - 150) ** 2 + (yy - 120) ** 2) / (2 * 55**2)))
    scene += 90 * np.exp(-(((xx - 340) ** 2 + (yy - 230) ** 2) / (2 * 70**2)))
    scene += 40 * np.sin(xx / 18.0) * np.cos(yy / 22.0)  # waves
    scene += rng.normal(0, 4, scene.shape)  # mild noise

    # Squeeze the whole scene into the narrow band [60, 170]  -> low contrast
    scene = (scene - scene.min()) / (scene.max() - scene.min())  # to [0, 1]
    scene = 60 + scene * (170 - 60)  # to [60,170]
    img = scene.astype(np.uint8)

    cv2.imwrite(str(out_path), img)
    return img


# ---------------------------------------------------------------------------
# 1) Histogram   (both ways shown in the notes: numpy + OpenCV)
# ---------------------------------------------------------------------------
def compute_histogram(gray: np.ndarray) -> np.ndarray:
    """Return the histogram n_k : count of pixels at each intensity k."""
    manual = np.bincount(gray.ravel(), minlength=L)  # numpy
    opencv = cv2.calcHist([gray], [0], None, [L], [0, L]).ravel()  # OpenCV
    # They must agree; use the OpenCV one (float counts) for the CDF later.
    assert np.allclose(manual, opencv), "histogram mismatch!"
    return opencv.astype(np.int64)


# ---------------------------------------------------------------------------
# 2) DEDUCE the stretching parameters r1, r2 FROM the histogram
# ---------------------------------------------------------------------------
def deduce_params(
    gray: np.ndarray,
    hist: np.ndarray,
    method: str = "percentile",
    low_pct: float = 2.0,
    high_pct: float = 98.0,
) -> tuple[int, int]:
    """
    Return (r1, r2): the input-intensity control points to be stretched
    onto [0, 255].
    """
    if method == "minmax":
        # --- formula exactly as in the notes, but sensitive to outliers ---
        return int(gray.min()), int(gray.max())

    if method == "percentile":
        # --- recommended by the notes warning: ignore the extreme 2% ------
        r1 = int(np.percentile(gray, low_pct))
        r2 = int(np.percentile(gray, high_pct))
        return r1, r2

    if method == "cdf":
        # --- read the parameters straight off the cumulative histogram -----
        cdf = np.cumsum(hist) / gray.size  # normalized CDF, 0..1
        r1 = int(np.searchsorted(cdf, low_pct / 100.0))  # first k: CDF>=p_low
        r2 = int(np.searchsorted(cdf, high_pct / 100.0))  # first k: CDF>=p_high
        return r1, r2

    raise ValueError(f"unknown method: {method!r}")


# ---------------------------------------------------------------------------
# 3) BUILD the Lookup Table (piecewise-linear contrast-stretch curve)
# ---------------------------------------------------------------------------
def build_stretch_lut(r1: int, r2: int) -> np.ndarray:
    """
    256-entry uint8 LUT implementing the 3-segment contrast-stretch curve:

        s(r) = 0                     r <= r1
        s(r) = 255/(r2-r1) * (r-r1)  r1 <  r <= r2      (slope > 1 = boost)
        s(r) = 255                   r >  r2

    The division-by-zero cases flagged in the notes (r1 == 0, r2 == 255)
    are guarded here.
    """
    # Degenerate ranges (flat image etc.) -> identity LUT, nothing to stretch
    if r2 <= r1:
        print(f"  [warn] r2 ({r2}) <= r1 ({r1}) -> using identity mapping")
        return np.arange(L, dtype=np.uint8)

    r = np.arange(L, dtype=np.float64)
    slope = MAX_LEVEL / (r2 - r1)  # middle-segment slope

    lut = np.empty(L, dtype=np.float64)
    lut[r <= r1] = 0.0  # segment 1 (s1=0)
    mid = (r > r1) & (r <= r2)
    lut[mid] = slope * (r[mid] - r1)  # segment 2 (steep)
    lut[r > r2] = MAX_LEVEL  # segment 3 (s2=255)

    # Point ops must be MONOTONIC for a sensible mapping, and of course 8-bit
    lut = np.clip(np.round(lut), 0, MAX_LEVEL).astype(np.uint8)
    assert np.all(np.diff(lut.astype(int)) >= 0), "LUT must be monotonic"
    return lut


# ---------------------------------------------------------------------------
# 4) APPLY the LUT  ->  this is where cv2.LUT does its one-pass magic
# ---------------------------------------------------------------------------
def apply_lut(img: np.ndarray, lut: np.ndarray) -> np.ndarray:
    """
    cv2.LUT(src, lut) replaces every pixel value v with lut[v].
      * gray image + lut shape (256,)     -> gray result
      * BGR image + lut shape (256, 1, 3) -> per-channel color result
        (a plain (256, 3) array is seen as a 2-D single-channel matrix
         and would be rejected for 3-channel sources)
    """
    return cv2.LUT(img, lut)


def color_stretch(img_bgr: np.ndarray, method: str, low: float, high: float):
    """
    Stretch a COLOR image: deduce (r1, r2) PER CHANNEL from its histogram
    and pack the three curves into one (256, 3) LUT so cv2.LUT does all
    channels in a single pass.
    """
    channels = cv2.split(img_bgr)
    luts, params = [], []
    for name, ch in zip(("B", "G", "R"), channels):  # OpenCV order: BGR
        hist = compute_histogram(ch)
        r1, r2 = deduce_params(ch, hist, method, low, high)
        params.append((name, r1, r2))
        luts.append(build_stretch_lut(r1, r2))
    # Shape must be (256, 1, 3): OpenCV treats a plain (256, 3) array as a
    # 2-D single-channel matrix, which cv2.LUT rejects for 3-channel sources.
    lut_3ch = np.stack(luts, axis=1).reshape(L, 1, 3)
    return apply_lut(img_bgr, lut_3ch), params


def contrast_metric(gray: np.ndarray) -> float:
    """Std-dev of the intensities = simple quantitative measure of contrast
    (a narrow histogram has small std, a spread-out one has large std)."""
    return float(gray.std())


# ---------------------------------------------------------------------------
# 5) Report + visualization
# ---------------------------------------------------------------------------
def print_report(title: str, r1: int, r2: int, before: np.ndarray, after: np.ndarray):
    span_in = r2 - r1
    slope = MAX_LEVEL / span_in if span_in > 0 else float("inf")
    print(f"  {title}")
    print(f"    deduced r1 (X_min)        : {r1}")
    print(f"    deduced r2 (X_max)        : {r2}")
    print(
        f"    input dynamic range       : {span_in} levels "
        f"(out of {L}, {100.0 * span_in / L:.1f}%)"
    )
    print(
        f"    middle-segment slope 255/(r2-r1) = {slope:.3f} "
        f"({'CONTRAST IS BOOSTED' if slope > 1 else 'no boost'})"
    )
    print(f"    contrast (std) before     : {contrast_metric(before):.2f}")
    print(f"    contrast (std) after      : {contrast_metric(after):.2f}")


def plot_results(
    orig_bgr: np.ndarray,
    stretched_bgr: np.ndarray,
    gray: np.ndarray,
    gray_stretched: np.ndarray,
    hist: np.ndarray,
    hist_stretched: np.ndarray,
    lut: np.ndarray,
    r1: int,
    r2: int,
    out_png: Path,
):
    """Full figure: images, histograms (with the deduced r1/r2 marked on the
    x-axis) and the transfer curve (the LUT itself)."""
    plt.rcParams.update(
        {"figure.facecolor": "white", "axes.grid": True, "grid.alpha": 0.3}
    )
    fig, ax = plt.subplots(2, 3, figsize=(15, 8))
    fig.suptitle(
        "Contrast stretching — histogram-deduced parameters "
        f"(r1={r1}, r2={r2}) applied with cv2.LUT",
        fontsize=14,
    )

    # -- row 0: the images ------------------------------------------------
    ax[0, 0].imshow(cv2.cvtColor(orig_bgr, cv2.COLOR_BGR2RGB))
    ax[0, 0].set_title(f"Original  (std={gray.std():.1f})")
    ax[0, 1].imshow(cv2.cvtColor(stretched_bgr, cv2.COLOR_BGR2RGB))
    ax[0, 1].set_title(f"Contrast-stretched  (std={gray_stretched.std():.1f})")
    ax[0, 2].plot(np.arange(L), lut, color="tab:green", lw=2)
    ax[0, 2].axvline(r1, color="red", ls="--", lw=1, label=f"r1={r1}")
    ax[0, 2].axvline(r2, color="blue", ls="--", lw=1, label=f"r2={r2}")
    ax[0, 2].set_title("Transfer curve s = T(r)  (the LUT)")
    ax[0, 2].set_xlabel("input r")
    ax[0, 2].set_ylabel("output s")
    ax[0, 2].set_xlim(0, 255)
    ax[0, 2].set_ylim(0, 255)
    ax[0, 2].set_aspect("equal")
    ax[0, 2].legend(loc="lower right")

    # -- row 1: the histograms -------------------------------------------
    ax[1, 0].bar(np.arange(L), hist, width=1.0, color="gray")
    for x, c, lab in ((r1, "red", "r1"), (r2, "blue", "r2")):
        ax[1, 0].axvline(x, color=c, ls="--", lw=1.5, label=lab)
    ax[1, 0].set_title("Original histogram  (narrow -> low contrast)")
    ax[1, 0].set_xlabel("intensity")
    ax[1, 0].set_ylabel("pixel count")
    ax[1, 0].legend()

    ax[1, 1].bar(np.arange(L), hist_stretched, width=1.0, color="tab:orange")
    ax[1, 1].set_title("Stretched histogram (spread over [0,255])")
    ax[1, 1].set_xlabel("intensity")
    ax[1, 1].set_ylabel("pixel count")

    # zoom on the active band so the "narrowness" is obvious
    pad = max(10, (r2 - r1) // 4)
    lo, hi = max(0, r1 - pad), min(L, r2 + pad + 1)
    ax[1, 2].bar(np.arange(lo, hi), hist[lo:hi], width=1.0, color="gray")
    ax[1, 2].axvline(r1, color="red", ls="--", lw=1.5, label="r1")
    ax[1, 2].axvline(r2, color="blue", ls="--", lw=1.5, label="r2")
    ax[1, 2].set_title(f"Zoom on used band [{lo}, {hi})")
    ax[1, 2].set_xlabel("intensity")
    ax[1, 2].legend()

    fig.tight_layout(rect=(0, 0, 1, 0.95))
    fig.savefig(out_png, dpi=130)
    plt.close(fig)


# ---------------------------------------------------------------------------
# 6) Main pipeline
# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(
        description="Histogram-driven contrast stretching via cv2.LUT"
    )
    ap.add_argument(
        "image",
        nargs="?",
        default=None,
        help="path to the image (default: generate a low-contrast sample)",
    )
    ap.add_argument(
        "--method",
        choices=("minmax", "percentile", "cdf"),
        default="percentile",
        help="how to deduce r1/r2 from the histogram "
        "(default: percentile = robust, as recommended in the notes)",
    )
    ap.add_argument(
        "--low", type=float, default=2.0, help="low percentile  (default 2)"
    )
    ap.add_argument(
        "--high", type=float, default=98.0, help="high percentile (default 98)"
    )
    ap.add_argument(
        "--outdir",
        default=str(Path(__file__).parent / "outputs"),
        help="where results are saved",
    )
    args = ap.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    # ---- load (or create) the corresponding image -----------------------
    if args.image is None:
        sample_path = outdir / "sample_low_contrast.png"
        img = make_low_contrast_sample(sample_path)
        src_desc = (
            f"auto-generated low-contrast sample ({sample_path.name}, band 60..170)"
        )
    else:
        img = cv2.imread(args.image, cv2.IMREAD_COLOR)
        if img is None:
            raise SystemExit(f"could not read image: {args.image}")
        src_desc = args.image

    # Normalize to 3-channel BGR (OpenCV's default) even if the source is
    # a grayscale file, so every downstream step can rely on the channel count.
    if img.ndim == 2:
        img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    print(f"[input] {src_desc}   shape={img.shape}")

    # ---- (1) histogram of the GRAY version (analyze luminance) ----------
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    hist = compute_histogram(gray)
    used = np.nonzero(hist)[0]
    print(
        f"[histogram] intensities actually present: {used.min()}..{used.max()} "
        f"({used.size} distinct levels of {L})"
    )

    # ---- (2) DEDUCE r1, r2 from that histogram --------------------------
    print(f"[deduce] method = {args.method}  (low={args.low}%, high={args.high}%)")
    r1, r2 = deduce_params(gray, hist, args.method, args.low, args.high)

    # ---- (3) build the LUT  &  (4) apply it with cv2.LUT ----------------
    lut = build_stretch_lut(r1, r2)
    gray_stretched = apply_lut(gray, lut)  # (256,) LUT
    color_stretched, per_ch = color_stretch(img, args.method, args.low, args.high)

    # ---- (5) report ------------------------------------------------------
    print_report("GRAYSCALE stretch", r1, r2, gray, gray_stretched)
    print("  COLOR stretch (per-channel parameters):")
    for (name, cr1, cr2), ch_before, ch_after in zip(
        per_ch, cv2.split(img), cv2.split(color_stretched)
    ):
        print(
            f"    ch {name}: r1={cr1:3d}  r2={cr2:3d}  "
            f"std {contrast_metric(ch_before):5.2f} -> {contrast_metric(ch_after):5.2f}"
        )

    # ---- (6) save everything useful --------------------------------------
    gray_bgr = cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)
    gray_str_bgr = cv2.cvtColor(gray_stretched, cv2.COLOR_GRAY2BGR)
    p = {
        "original": outdir / "original.png",
        "stretched_gray": outdir / "stretched_gray.png",
        "stretched_color": outdir / "stretched_color.png",
        "side_by_side": outdir / "side_by_side.png",
        "analysis": outdir / "histogram_analysis.png",
        "lut": outdir / "stretch_lut.csv",
    }
    cv2.imwrite(str(p["original"]), img)
    cv2.imwrite(str(p["stretched_gray"]), gray_stretched)
    cv2.imwrite(str(p["stretched_color"]), color_stretched)
    cv2.imwrite(str(p["side_by_side"]), np.hstack([gray_bgr, gray_str_bgr]))
    # Save the LUT as explicit (input_intensity, output_intensity) pairs
    np.savetxt(
        p["lut"],
        np.column_stack([np.arange(L), lut]),
        fmt="%d",
        header="input_intensity,output_intensity",
        comments="",
        delimiter=",",
    )
    plot_results(
        img,
        color_stretched,
        gray,
        gray_stretched,
        hist,
        compute_histogram(gray_stretched),
        lut,
        r1,
        r2,
        p["analysis"],
    )

    print("[saved]")
    for f in p.values():
        print(f"    {f}")


if __name__ == "__main__":
    main()
