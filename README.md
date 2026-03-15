# Digital Image Correlation (DIC) Engine
**Sub-pixel Displacement and Strain Measurement System**

![C++](https://img.shields.io/badge/C++-14-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.10+-brightgreen.svg)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-red.svg)
![Python](https://img.shields.io/badge/Python-3.x-yellow.svg)

## Project Overview
This project is a Digital Image Correlation (DIC) engine developed for sub-pixel displacement measurement in experimental mechanics and the validation of engineering simulations (e.g., Finite Element Analysis). 

It utilizes a decoupled architecture, employing C++ for core computational tasks and Python for pipeline scheduling and data visualization. The system is designed to provide a reliable, automated workflow from image processing to final result rendering.

## Core Features
* **Backend Engine (C++)**: Object-oriented design (`DICMeshField`, `DICPostProcessor`) focused on efficient memory management and computation.
* **Algorithmic Implementation**:
* **Coarse Search**: Fast Normalized Cross-Correlation (FNCC).
* **Sub-pixel Fine Search**: Inverse Compositional Gauss-Newton (ICGN) iteration, including the calculation of 6-parameter Jacobian matrices.
* **Data Processing**: Integrates an 8-neighborhood repair algorithm to handle invalid data points caused by experimental noise or local decorrelation.
* **Automated Pipeline**: Managed by CMake and Python, isolating the intensive computational engine from the visualization layer.

## System Architecture
The workflow consists of three main stages:
1. **Image Generator (`Image_Gen.exe`)**: Generates simulated speckle patterns with predefined ground-truth deformations using affine transformations.
2. **DIC Core Engine (`DIC_App.exe`)**: Reads the speckle images, executes the FNCC and ICGN algorithms, and exports the displacement data as a CSV file.
3. **Python Controller (`run_pipeline.py`)**: Triggers the C++ executables, displays intermediate visual checkpoints, and renders the final 4D heatmaps (U, V, UX, UY, VX, VY).

## Visualization & Results
*(Please replace the placeholders below with your generated speckle images and heatmap matrix)*

*Figure 1: Reference vs. Deformed Speckle Images*
![Speckle Images](placeholder_link_for_speckle_images)

*Figure 2: Full-field Displacement and Strain Cloud Maps*
![Cloud Map](placeholder_link_for_cloud_map)

## Quick Start

### Prerequisites
* CMake (>= 3.10)
* Visual Studio (MSVC) or GCC
* OpenCV (>= 4.0)
* Python 3 (with `pandas`, `matplotlib`, `numpy`)

### Build Instructions
```bash
# Generate build files
cmake -B build

# Compile the executables in Release mode
cmake --build build --config Release
