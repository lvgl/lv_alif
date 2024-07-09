# Alif M55 LVGL Demo

## Description
[LVGL](https://github.com/lvgl/lvgl) example for Alif E7 DevKit Gen2

This app is porting the LVGL [v9.1.0](https://github.com/lvgl/lvgl/releases/tag/v9.1.0) to Alif Cortex-M55 with D/AVE 2D GPU.
It launches the Demo Benchmark example to measure its performance on the M55 HP and HE cores using GPU acceleration and FreeRTOS.

Benchmark Summary: LVGL v9.1.0, M55 HP core, D/AVE 2D, 60FPS display framerate

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| ------------------------- |:--------:|:--------:|:---------:|:-----------:| ----------:|
| Empty screen              | 14%      | 54       | 12        | 1           | 11         |
| Moving wallpaper          | 13%      | 61       | 14        | 6           | 8          |
| Single rectangle          | 1%       | 60       | 15        | 0           | 15         |
| Multiple rectangles       | 11%      | 60       | 15        | 1           | 14         |
| Multiple RGB images       | 23%      | 60       | 14        | 3           | 11         |
| Multiple ARGB images      | 24%      | 60       | 13        | 6           | 7          |
| Rotated ARGB images       | 13%      | 30       | 30        | 23          | 7          |
| Multiple labels           | 42%      | 60       | 15        | 7           | 8          |
| Screen sized text         | 84%      | 30       | 31        | 28          | 3          |
| Multiple arcs             | 21%      | 61       | 13        | 2           | 11         |
| Containers                | 15%      | 60       | 14        | 2           | 12         |
| Containers with overlay   | 47%      | 60       | 15        | 10          | 5          |
| Containers with opa       | 15%      | 60       | 14        | 2           | 12         |
| Containers with opa_layer | 44%      | 56       | 16        | 9           | 7          |
| Containers with scrolling | 53%      | 60       | 14        | 9           | 5          |
| Widgets demo              | 48%      | 37       | 21        | 10          | 11         |
| All scenes avg.           | 29%      | 54       | 16        | 7           | 9          |

## Requirements
This application is built on [VSCode Getting Started Template](https://github.com/alifsemi/alif_vscode-template).

The required software setup consists of VSCode, Git, CMake, cmsis-toolbox, Arm GNU toolchain and Alif tools.
Please refer to the [Getting Started Guide](https://alifsemi.com/download/AUGD0012) for more details.

This app also requires following CMSIS packs to be installed and added to the project:
  * `ARM::CMSIS@5.9.0` (https://github.com/ARM-software/CMSIS_5/releases/tag/5.9.0)
  * `ARM::CMSIS-FreeRTOS@10.5.1` (https://github.com/ARM-software/CMSIS-FreeRTOS/releases/tag/v10.5.1)
  * `LVGL::lvgl@9.1.0` (https://github.com/lvgl/lvgl/tree/release/v9.1/env_support/cmsis-pack)
  * `AlifSemiconductor::Ensemble@1.1.1` (https://github.com/alifsemi/alif_ensemble-cmsis-dfp/releases/tag/v1.1.1)
  * `AlifSemiconductor::Dave2DDriver@1.0.0` (https://github.com/alifsemi/alif_dave2d-driver)
  * `AlifSemiconductor::LVGL_DAVE2D@1.0.0` (https://github.com/alifsemi/alif_lvgl-dave2d)

By default, these packs are installed VS Code `First time pack installation` script (see below).

The default hardware is [Alif Ensemble DevKit Gen 2](https://alifsemi.com/support/kits/ensemble-devkit-gen2/) with display.

## Get started
To build the app you need to clone this repository:
```
git clone --recursive https://github.com/alifsemi/alif_m55-lvgl
```

After setting up the environment according to the [Getting Started Guide](https://alifsemi.com/download/AUGD0012) you can select **File->Open Folder** from VSCode and press **F1** and start choosing from the preset build tasks.

1. **F1** --> Tasks:Run Task --> First time pack installation
2. **F1** --> Tasks:Run Task --> Generate and Build with csolution + cbuild
3. **F1** --> Tasks:Run Task --> Program with Security Toolkit
