# INF2004 Autonomous Car Project

# Requirements
- [Pico SDK v1.5.1](https://github.com/raspberrypi/pico-sdk/)
- Pico ARM Toolchain
    - CMake v3.13+
    - ARM GCC EABI v10.3.1
- Pico W board
- [FreeRTOS Kernel v10.6.1](https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/tag/V10.6.1)

## Environment Variables

| Variable | Description |
| -- | -- |
| `PICO_SDK_PATH` | Path to the Pico SDK directory |
| `FREERTOS_KERNEL_PATH` | Path to the FreeRTOS Kernel directory |
| `WIFI_SSID` | Wi-Fi network SSID |
| `WIFI_PASSWORD` | Wi-Fi network password |


# Compilation

## CMake Configuration
```bash
cmake -S <current directory> -B <build directory>
```

## CMake Build
```bash
cmake --build <build directory> --config <Debug/Release> --target car_project -j 18 --
```