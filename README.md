# spmon - Small Performance Monitor

This tool reads near-realtime (interval: 1s) system usage statistics and display them in the terminal.
Currently, it shows:
- CPU usage and temperature
- RAM usage
- GPU usage and temperature
- Network usage (up/down)

## Supported hardware

Currently only NVidia GPUs are supported.
This is due to the fact an NVidia library is used to get data from the GPU.

## Building

Requires `ncurses` and `nvidia-ml` libraries.
With those installed, simply running `make` should produce an `spmon` executable.