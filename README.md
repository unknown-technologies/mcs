Main Console System
===================

This is the source code of the Emulight MCS. It is a GUI program which directly
uses OpenGL ES + EGL to draw graphics without X11/wayland and it directly uses
the Linux input system for multitouch handling.


Features
--------

- Clock
- Configurable alarm
- (Planned: a lot more)


System Requirements
-------------------

- gcc, make, git
- glslang
- [bin2o](https://github.com/hackyourlife/bin2o)
- installation of OpenGL ES + EGL
- Raspberry Pi 4 (1GB model is sufficient)
- user with permission for direct audio/input/video


Building
--------

Just run `make`. If you build on the RPi 4 directly, `make -j4` might be more
useful.
