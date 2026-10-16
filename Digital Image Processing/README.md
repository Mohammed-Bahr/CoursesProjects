---
created: 2026-10-11
modified: 2026-10-11
tags:
  - FACL
  - FACL/DIP/Foundations
  - Project
Doc Lec order: LEC 2
status: Done
---

# Project — Contrast stretching via histogram + `cv2.LUT`

> [!quote] Task
> **Use the image histogram to deduce the parameters for contrast stretching, then apply contrast stretching on the corresponding image.**
> Hint: **`cv2.LUT`**

Implements exactly what `00.Foundations&Enhancement of DIP.md` §2.4(4) + §2.5 teach:

1. **Histogram** of the image is computed (`np.bincount` ✔ and `cv2.calcHist` ✔, both shown in the notes and cross-checked against each other).
2. **Parameters are deduced from that histogram** — the two control points `(r1, 0)` and `(r2, 255)` of the piecewise-linear curve, with three deduction methods:

   | `--method`    | How r1, r2 are read from the histogram                                                                 |
   | ------------- | ------------------------------------------------------------------------------------------------------ |
   | `minmax`      | `r1 = gray.min()`, `r2 = gray.max()` — the exact notes formula, but one outlier pixel ruins the range   |
   | `percentile`  | **default** — `r1`/`r2` at the 2%/98% percentiles; the robust version the notes' ⚠️ warning recommends |
   | `cdf`         | first intensity where the normalized CDF crosses `low%` / `high%` — read straight off the cumulative histogram |

3. **LUT built** from the 3-segment stretch curve (division-by-zero cases guarded, monotonicity asserted):

   ```
   s(r) = 0                    r <= r1
   s(r) = 255/(r2-r1) * (r-r1) r1 <  r <= r2   (slope > 1 → contrast boost)
   s(r) = 255                  r >  r2
   ```

4. **Applied in one pass with `cv2.LUT`** — grayscale uses a `(256,)` LUT; color uses a per-channel `(256, 1, 3)` LUT (OpenCV rejects a plain `(256, 3)` array for 3-channel sources).

## Run

```bash
cd Project

# no image at hand? generates a low-contrast sample (band 60..170) automatically
python3 contrast_stretching.py

# your own image
python3 contrast_stretching.py path/to/image.jpg
python3 contrast_stretching.py image.png --method minmax
python3 contrast_stretching.py image.png --method cdf --low 1 --high 99
```

Dependencies: `opencv-python`, `numpy`, `matplotlib` (headless `Agg` backend — works over SSH/terminals).

## Outputs (`outputs/`)

| File                          | Content                                                            |
| ----------------------------- | ------------------------------------------------------------------ |
| `histogram_analysis.png`      | before/after images + histograms (r1/r2 marked) + the LUT curve    |
| `original.png` / `stretched_gray.png` / `stretched_color.png` | the images themselves              |
| `side_by_side.png`            | gray original ∥ gray stretched                                      |
| `stretch_lut.csv`             | the full 256-entry transformation table `(input, output)`          |
| `sample_low_contrast.png`     | the auto-generated demo image                                      |

## Sample result (percentile method, auto sample)

- histogram occupies only **60..170** → deduced `r1 = 75`, `r2 = 149` (74 of 256 levels ≈ **29%** of the dynamic range)
- middle-segment slope `255/74 ≈ 3.45` → contrast **is** boosted
- contrast (std) **17.9 → 60.3** after stretching — the washed-out image becomes vivid, exactly like the pollen-image slide example in the lecture
