# INF2004 Autonomous Car Project
[![Tests](https://github.com/ForceLightning/INF2004-Project/actions/workflows/ctest.yml/badge.svg)](https://github.com/ForceLightning/INF2004-Project/actions/workflows/ctest.yml) [![Docs](https://github.com/ForceLightning/INF2004-Project/actions/workflows/docs.yml/badge.svg)](https://github.com/ForceLightning/INF2004-Project/actions/workflows/docs.yml) [![Works](https://forcelightning-cdn.sgp1.digitaloceanspaces.com/works_on_my_machine.svg)](https://github.com/nikku/works-on-my-machine)
# Requirements
- [Pico SDK v1.5.1](https://github.com/raspberrypi/pico-sdk/)
- Pico ARM Toolchain
    - CMake v3.13+
    - ARM GCC EABI v10.3.1
- Pico W board
- [FreeRTOS Kernel v10.6.1](https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/tag/V10.6.1)

## Environment Variables

| Variable               | Description                           |
| ---------------------- | ------------------------------------- |
| `PICO_SDK_PATH`        | Path to the Pico SDK directory        |
| `FREERTOS_KERNEL_PATH` | Path to the FreeRTOS Kernel directory |
| `WIFI_SSID`            | Wi-Fi network SSID                    |
| `WIFI_PASSWORD`        | Wi-Fi network password                |
| `TEST_BUILD`           | Define to enable `ctest` testing      |


# Compilation

## CMake Configuration
```bash
cmake -S <current directory> -B <build directory>
```

## CMake Build
```bash
cmake --build <build directory> --config <Debug/Release> --target car_project -j 18 --
```

# Testing
Unit testing of code that is independent of the Pico SDK is done with `ctest` by defining the environment variable `TEST_BUILD`. Some configuration changes to your environment will be required such as changing the compiler toolkit so that the tests may run on your machine.

# Documentation
Documentation can be found [here](https://forcelightning.github.io/INF2004-Project/).

# Diagrams
## Block Diagram
![Block Diagram](images/blockdiagram.svg)

## Call Graph
![Call graph](images/callgraph.svg)

## Data Flow
![Data flow](images/dataflow.svg)

# Contributing
The enforced coding standard is the [Embedded C Coding Standard by the Barr Group](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard). See the [Table of Abbreviations](ABBREV.md).

# Licence
[BSD-3-Clause](LICENSE.txt)