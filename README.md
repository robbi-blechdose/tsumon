# tsumon - Terminal System Usage MONitor

This tool reads near-realtime (interval: 0.25s - 5s) system usage statistics and display them in the terminal.
Currently, it shows:
- CPU usage and temperature
- RAM usage
- GPU usage and temperature, GPU memory usage
- Network usage (up/down)
- Disk usage (read/write)
- System information (OS, kernel, hostname, uptime)

## Supported hardware

GPU monitoring is currently only supported on NVidia GPUs.
tsumon can be run on non-NVidia systems as well with disabled GPU monitoring.

## Building

Requires `ncurses` and `nvidia-ml` libraries.
With those installed, simply running `make` should produce an `tsumon` executable.