#!/usr/bin/env python3
"""
Digital Image Processing - Lab 2
Histogram Analysis & Contrast Stretching

Experiment 1: histogram, histogram shift, cumulative histogram, equalization
Experiment 2: deduce contrast-stretching parameters from the histogram, apply with cv2.LUT

Usage:
    python lab2_histogram.py                      # uses a built-in synthetic low-contrast image
    python lab2_histogram.py path/to/image.png    # your own image (color is converted to gray)
    python lab2_histogram.py img.png --shift 60 --low 1 --high 99
    python lab2_histogram.py img.png --no-show    # only save figures (no pop-up windows)

Figures are saved to ./lab2_output (change with --out).
"""

import argparse
import os
import sys

import numpy as np
import cv2
import matplotlib

# The backend must be chosen before pyplot is imported.
if "--no-show" in sys.argv:
    matplotlib.use("Agg")
import matplotlib.pyplot as plt


# --------------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------------
def load_gray(path):
    """Load an image as 8-bit grayscale (shape (M, N), dtype uint8)."""
    img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        sys.exit(
            f"Error: could not read image '{path}'. Check the path and file format."
        )
    return img


def make_demo_image(h=300, w=400, seed=0):
    """Synthetic low-contrast image (values roughly 90..170) so the lab runs without any file."""
    rng = np.random.default_rng(seed)
    y, x = np.mgrid[0:h, 0:w]
    base = 100 + 40 * (x / w)  # soft horizontal gradient
    disk = ((x - w * 0.3) ** 2 + (y - h * 0.5) ** 2) < (h * 0.25) ** 2
    rect = (abs(x - w * 0.72) < w * 0.15) & (abs(y - h * 0.5) < h * 0.2)
    img = base + 25 * disk - 20 * rect + rng.normal(0, 4, (h, w))  # shapes + mild noise
    return np.clip(img, 0, 255).astype(np.uint8)


def compute_hist(img):
    """Histogram n_j for j = 0..255, using cv2.calcHist (returns a flat float array)."""
    return cv2.calcHist(
        [img], [0], None, [256], [0, 256]
    ).ravel()  # (image , channal zero , no masks , number of bins , the range from histogram to flattern matrix)


def cumulative_hist(hist):
    """Cumulative histogram: number of pixels with value <= r."""
    return np.cumsum(hist)


def shift_histogram(img, value):
    """
    Histogram shift by `value`: s = r + value, saturated to [0, 255].
    Done through a lookup table so it is applied the same way as everything else.
    Pixels pushed past 0 or 255 pile up at the border (information is lost).
    """
    lut = np.clip(np.arange(256) + value, 0, 255).astype(np.uint8)
    return cv2.LUT(img, lut)


def equalize_manual(img):
    """Textbook formula s = round(255 / (M*N) * sum_{j<=r} n_j), used to cross-check cv2.equalizeHist."""
    cdf = cumulative_hist(compute_hist(img))
    lut = np.round(255.0 * cdf / img.size).astype(np.uint8)
    return cv2.LUT(img, lut)


def deduce_stretch_params(hist, low_pct, high_pct):
    """
    Read r_min / r_max from the cumulative histogram.
    r_min = smallest level where at least low_pct % of pixels are at or below it
    r_max = smallest level where at least high_pct % of pixels are at or below it
    With low=0 and high=100 these are the true min and max of the image.
    Using e.g. 1 / 99 makes the stretch robust to a few outlier pixels.
    """
    cdf = cumulative_hist(hist)
    total = cdf[-1]
    r_min = int(np.searchsorted(cdf, max(total * low_pct / 100.0, 1), side="left"))
    r_max = int(np.searchsorted(cdf, total * high_pct / 100.0, side="left"))
    return r_min, min(r_max, 255)


def stretch_lut(r_min, r_max):
    """256-entry LUT for s = (r - r_min) / (r_max - r_min) * 255, clipped to [0, 255]."""
    r = np.arange(256, dtype=np.float64)
    s = (r - r_min) / (r_max - r_min) * 255.0
    return np.round(np.clip(s, 0, 255)).astype(np.uint8)


# --------------------------------------------------------------------------
# Plot helpers
# --------------------------------------------------------------------------
def show_img(ax, img, title):
    ax.imshow(
        img, cmap="gray", vmin=0, vmax=255
    )  # fixed range so brightness is comparable
    ax.set_title(title)
    ax.axis("off")


def show_hist(ax, hist, title, color="tab:blue"):
    ax.bar(np.arange(256), hist, width=1.0, color=color)
    ax.set_xlim(0, 255)
    ax.set_title(title)
    ax.set_xlabel("Intensity")
    ax.set_ylabel("Pixel count")


def show_cumhist(ax, cdf, title, color="tab:red"):
    ax.plot(np.arange(256), cdf, color=color)
    ax.set_xlim(0, 255)
    ax.set_ylim(0, cdf[-1] * 1.02)
    ax.set_title(title)
    ax.set_xlabel("Intensity")
    ax.set_ylabel("Cumulative count")
    ax.grid(alpha=0.3)


def save(fig, out_dir, name):
    fig.tight_layout()
    path = os.path.join(out_dir, name)
    fig.savefig(path, dpi=120)
    print(f"  saved {path}")


# --------------------------------------------------------------------------
# Experiments
# --------------------------------------------------------------------------
def experiment_1(img, shift_value, out_dir):
    print("\n=== Experiment 1: histogram analysis & equalization ===")
    hist = compute_hist(img)
    cdf = cumulative_hist(hist)
    print(
        f"  image size: {img.shape[1]}x{img.shape[0]}  min={img.min()}  max={img.max()}  mean={img.mean():.1f}"
    )

    # 1a. original image, its histogram, its cumulative histogram
    fig, ax = plt.subplots(1, 3, figsize=(15, 4))
    fig.suptitle("Experiment 1a - Original image")
    show_img(ax[0], img, "Original image")
    show_hist(ax[1], hist, "Histogram")
    show_cumhist(ax[2], cdf, "Cumulative histogram")
    save(fig, out_dir, "exp1a_original.png")

    # 1b. histogram shift by value
    shifted = shift_histogram(img, shift_value)
    hist_sh = compute_hist(shifted)
    clipped = int(
        hist_sh[0] if shift_value < 0 else hist_sh[255] if shift_value > 0 else 0
    )
    print(
        f"  shift by {shift_value:+d}: new min={shifted.min()} max={shifted.max()}  "
        f"pixels piled on the clipped border: {clipped}"
    )
    fig, ax = plt.subplots(2, 2, figsize=(11, 8))
    fig.suptitle(f"Experiment 1b - Histogram shift by {shift_value:+d}")
    show_img(ax[0, 0], img, "Original")
    show_hist(ax[0, 1], hist, "Original histogram")
    show_img(ax[1, 0], shifted, f"Shifted ({shift_value:+d})")
    show_hist(ax[1, 1], hist_sh, "Shifted histogram", color="tab:orange")
    save(fig, out_dir, "exp1b_shift.png")

    # 1c. histogram equalization (cv2.equalizeHist)
    eq = cv2.equalizeHist(img)
    hist_eq = compute_hist(eq)
    cdf_eq = cumulative_hist(hist_eq)

    # cross-check against the textbook formula (they differ by at most a few gray levels,
    # because OpenCV subtracts cdf_min so the darkest occupied level maps exactly to 0)
    diff = np.abs(eq.astype(int) - equalize_manual(img).astype(int))
    print(f"  equalized: min={eq.min()} max={eq.max()}  mean={eq.mean():.1f}")
    print(
        f"  cv2.equalizeHist vs textbook formula: max |diff| = {diff.max()} gray levels"
    )

    fig, ax = plt.subplots(2, 3, figsize=(15, 8))
    fig.suptitle("Experiment 1c - Histogram equalization (top: before, bottom: after)")
    show_img(ax[0, 0], img, "Original image")
    show_hist(ax[0, 1], hist, "Original histogram")
    show_cumhist(ax[0, 2], cdf, "Original cumulative histogram")
    show_img(ax[1, 0], eq, "Equalized image")
    show_hist(ax[1, 1], hist_eq, "Equalized histogram", color="tab:green")
    show_cumhist(ax[1, 2], cdf_eq, "Equalized cumulative histogram", color="tab:purple")
    save(fig, out_dir, "exp1c_equalization.png")
    return eq


def experiment_2(img, low_pct, high_pct, out_dir):
    print("\n=== Experiment 2: contrast stretching ===")
    hist = compute_hist(img)
    r_min, r_max = deduce_stretch_params(hist, low_pct, high_pct)
    print(f"  true min/max of image : {img.min()} / {img.max()}")
    print(
        f"  chosen percentiles    : {low_pct}% / {high_pct}%  ->  r_min={r_min}, r_max={r_max}"
    )

    if r_max <= r_min:
        print(
            "  WARNING: r_max <= r_min (flat image or bad percentiles). Skipping stretch."
        )
        return img

    gain = 255.0 / (r_max - r_min)
    print(
        f"  s = (r - {r_min}) / ({r_max} - {r_min}) * 255      contrast gain = {gain:.2f}"
    )

    lut = stretch_lut(r_min, r_max)
    out = cv2.LUT(img, lut)  # apply the lookup table to every pixel
    hist_out = compute_hist(out)
    print(f"  stretched: min={out.min()} max={out.max()}  mean={out.mean():.1f}")

    fig, ax = plt.subplots(2, 3, figsize=(15, 8))
    fig.suptitle(f"Experiment 2 - Contrast stretching  (r_min={r_min}, r_max={r_max})")
    show_img(ax[0, 0], img, "Original image")
    show_hist(ax[0, 1], hist, "Original histogram")
    for v, lab in ((r_min, "r_min"), (r_max, "r_max")):
        ax[0, 1].axvline(v, color="red", linestyle="--")
        ax[0, 1].text(v, ax[0, 1].get_ylim()[1] * 0.92, f" {lab}={v}", color="red")
    ax[0, 2].plot(np.arange(256), lut, color="black")
    ax[0, 2].plot([0, 255], [0, 255], "--", color="gray", label="identity")
    ax[0, 2].set_title("Transformation s = T(r)  (the LUT)")
    ax[0, 2].set_xlabel("Input r")
    ax[0, 2].set_ylabel("Output s")
    ax[0, 2].set_xlim(0, 255)
    ax[0, 2].set_ylim(0, 255)
    ax[0, 2].legend()
    ax[0, 2].grid(alpha=0.3)
    show_img(ax[1, 0], out, "Stretched image")
    show_hist(ax[1, 1], hist_out, "Stretched histogram", color="tab:green")
    show_img(ax[1, 2], cv2.equalizeHist(img), "For comparison: equalized image")
    save(fig, out_dir, "exp2_stretching.png")
    return out


# --------------------------------------------------------------------------
def main():
    p = argparse.ArgumentParser(
        description="Lab 2: histogram analysis & contrast stretching"
    )
    p.add_argument(
        "image", nargs="?", help="path to an image (default: synthetic demo image)"
    )
    p.add_argument(
        "--shift",
        type=int,
        default=50,
        help="histogram shift value for Exp 1 (default 50)",
    )
    p.add_argument(
        "--low",
        type=float,
        default=1.0,
        help="lower percentile for stretching (default 1)",
    )
    p.add_argument(
        "--high",
        type=float,
        default=99.0,
        help="upper percentile for stretching (default 99)",
    )
    p.add_argument("--out", default="lab2_output", help="output folder for figures")
    p.add_argument(
        "--no-show", action="store_true", help="save figures without opening windows"
    )
    args = p.parse_args()

    if not (0 <= args.low < args.high <= 100):
        sys.exit("Error: need 0 <= --low < --high <= 100")

    os.makedirs(args.out, exist_ok=True)
    if args.image:
        img = load_gray(args.image)
    else:
        print("No image given -> using a synthetic low-contrast demo image.")
        img = make_demo_image()
        cv2.imwrite(os.path.join(args.out, "demo_input.png"), img)

    experiment_1(img, args.shift, args.out)
    stretched = experiment_2(img, args.low, args.high, args.out)
    cv2.imwrite(os.path.join(args.out, "stretched_result.png"), stretched)
    print(f"\nDone. Figures and result images are in '{args.out}'.")

    if not args.no_show:
        plt.show()


if __name__ == "__main__":
    main()
