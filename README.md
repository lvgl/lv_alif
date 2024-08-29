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
Please make sure you have setup your VSCode and other tools and environment based on this template and test it out by building and running the application.

The required software setup consists of VSCode, Git, CMake, cmsis-toolbox, Arm GNU toolchain and Alif tools.

This app also requires following CMSIS packs to be installed and added to the project:
  * `ARM::CMSIS@>=5.9.0` (https://github.com/ARM-software/CMSIS_5/releases/tag/5.9.0)
  * `ARM::CMSIS-FreeRTOS@10.5.1` (https://github.com/ARM-software/CMSIS-FreeRTOS/releases/tag/v10.5.1)
  * `LVGL::lvgl@9.1.0` (https://github.com/lvgl/lvgl/tree/release/v9.1/env_support/cmsis-pack)
  * `AlifSemiconductor::Ensemble@>=1.1.1` (https://github.com/alifsemi/alif_ensemble-cmsis-dfp/releases/tag/v1.1.1)
  * `AlifSemiconductor::Dave2DDriver@1.0.1` (https://github.com/alifsemi/alif_dave2d-driver)
  * `AlifSemiconductor::LVGL_DAVE2D@1.0.1` (https://github.com/alifsemi/alif_lvgl-dave2d)

By default, these packs are installed VS Code `First time pack installation` script (see below).

The default hardware is [Alif Ensemble DevKit Gen 2](https://alifsemi.com/support/kits/ensemble-devkit-gen2/) with display.

## Get started
To build the app you need to clone this repository:
```
git clone --recursive https://github.com/alifsemi/alif_m55-lvgl.git
```

After setting up the environment according to the [VSCode Getting Started Template](https://github.com/alifsemi/alif_vscode-template) you can select **File->Open Folder** from VSCode and press **F1** and start choosing from the preset build tasks.

1. **F1** --> Tasks:Run Task --> First time pack installation
2. **F1** --> Tasks:Run Task --> cmsis-csolution.build:Build (Better to do this from the CMSIS Extension Build (hammer icon))
3. **F1** --> Tasks:Run Task --> Program with Security Toolkit

#### Dave2DDriver and LVGL_DAVE2D packs installation
If you need to install `AlifSemiconductor::Dave2DDriver@1.0.1` and `AlifSemiconductor::LVGL_DAVE2D@1.0.1` manually, follow next steps:
1. Download `AlifSemiconductor.Dave2DDriver.1.0.1.pack` from https://github.com/alifsemi/alif_dave2d-driver
2. Download `AlifSemiconductor.LVGL_DAVE2D.1.0.1.pack` from https://github.com/alifsemi/alif_lvgl-dave2d
3. Open VS Code Terminal: **Terminal** --> **New terminal**
4. Enter the directory where `AlifSemiconductor.Dave2DDriver.1.0.1.pack` and `AlifSemiconductor.LVGL_DAVE2D.1.0.1.pack` are located
5. Execute commands `cpackget add AlifSemiconductor.Dave2DDriver.1.0.1.pack; cpackget add AlifSemiconductor.LVGL_DAVE2D.1.0.1.pack`
