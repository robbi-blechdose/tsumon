# tsumon - Terminal System Usage MONitor

This tool reads near-realtime (interval: 0.25s - 5s) system usage statistics and display them in the terminal.
Currently, it shows:
- CPU usage and temperature
- RAM usage
- GPU usage and temperature, GPU memory usage
- Network usage (up/down)
- System information (OS, kernel, hostname, uptime)

## Supported hardware

Currently only NVidia GPUs are supported.
This is due to the fact an NVidia library is used to get data from the GPU.

## Building

Requires `ncurses` and `nvidia-ml` libraries.
With those installed, simply running `make` should produce an `tsumon` executable.