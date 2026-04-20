# Assignment 2 – Canny Edge Detection

**Course:** CAP 4453 – Robot Vision  
**Language:** C  
**Topic:** Multi-stage edge detection using the Canny algorithm

---

## Overview

This project implements the full Canny edge detection pipeline from scratch in C. Unlike simple Sobel filtering, Canny produces clean, thin, well-localized edges by combining Gaussian smoothing, gradient computation, non-maximum suppression, and hysteresis thresholding. A second file (`marrh.c`) implements the related Marr-Hildreth edge detector for comparison.

---

## How It Works

The Canny pipeline is implemented in four stages:

1. **Gaussian Derivative Convolution**  
   Applies directional Gaussian derivative masks (controlled by sigma `σ`) to smooth the image and compute gradient components `Gx` and `Gy`.

2. **Gradient Magnitude**  
   Computes `|G| = sqrt(Gx² + Gy²)` and normalizes to 0–255, saved as `magnitude.pgm`.

3. **Ridge Peak Detection (Non-Maximum Suppression)**  
   Suppresses non-peak gradient values by checking neighbors along the gradient direction. The slope is used to classify the edge orientation into one of four directions (horizontal, vertical, diagonal ±45°). Result saved as `peaks.pgm`.

4. **Automatic Double Thresholding + Hysteresis**  
   - Automatically determines high (`hi`) and low (`lo = 0.35 × hi`) thresholds from the image histogram using a configurable `percent` parameter
   - Pixels above `hi` are immediately marked as edges
   - Pixels between `lo` and `hi` are kept only if connected to a strong edge (hysteresis)
   - Final result saved as `finaledges.pgm`

---

## Project Structure

```
assignment2_canny/
├── canny.c                  # Full Canny edge detection implementation
├── marrh.c                  # Marr-Hildreth edge detector implementation
├── garb34.pgm               # Test input image
├── magnitude.pgm            # Output: gradient magnitude
├── peaks.pgm                # Output: after non-maximum suppression
├── finaledges.pgm           # Output: final edges after hysteresis
└── example outputs/         # Reference outputs for validation
    ├── cannymag.pgm
    ├── cannypeaks.pgm
    └── cannyfinal.pgm
```

---

## Build & Run

**Compile Canny:**
```bash
gcc canny.c -o canny -lm
```

**Run:**
```bash
./canny <input.pgm> <magnitude.pgm> <peaks.pgm> <finaledges.pgm> <sigma> <percent>
```

**Example:**
```bash
./canny garb34.pgm magnitude.pgm peaks.pgm finaledges.pgm 1.0 0.05
```

**Compile Marr-Hildreth:**
```bash
gcc marrh.c -o marrh -lm
```

---

## Parameters

| Parameter | Description |
|-----------|-------------|
| `sigma`   | Controls Gaussian smoothing width. Higher = more blur, fewer edges. |
| `percent` | Fraction of pixels used to auto-determine the high threshold (e.g. `0.05` = top 5%). |

---

## Output Files

| File            | Description                                      |
|-----------------|--------------------------------------------------|
| `magnitude.pgm` | Normalized gradient magnitude image              |
| `peaks.pgm`     | Edges after non-maximum suppression              |
| `finaledges.pgm`| Final binary edge map after hysteresis           |

---

## Key Concepts Demonstrated

- Gaussian derivative filter construction and 2D convolution
- Gradient magnitude and direction computation
- Non-maximum suppression using slope-based neighbor comparison
- Histogram-based automatic threshold selection
- Hysteresis edge linking for connected, clean edge maps
- Comparison with Marr-Hildreth (Laplacian of Gaussian) edge detection