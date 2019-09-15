[![Build Status](https://travis-ci.com/lpenz/gkrellm-sdnotify.svg?branch=master)](https://travis-ci.com/lpenz/gkrellm-sdnotify) 
[![Github workflow](https://github.com/lpenz/gkrellm-sdnotify/workflows/CI/badge.svg)](https://github.com/lpenz/gkrellm-sdnotify/actions)

gkrellm-sdnotify
================

## Introduction

**gkrellm-sdnotify** is a plugin for [gkrellm](http://gkrellm.srcbox.net/)
that calls
[systemd's](https://www.freedesktop.org/wiki/Software/systemd/)
[sd_notify (3)](https://www.freedesktop.org/software/systemd/man/sd_notify.html)
when loaded, after a user-defined delay.

With the help of this plugin, you can run gkrellm as a user systemd
service unit of `Type=notify`, and orchestrate other system units to
start after it without using timers.



## Installation


### From source

Install the following packages in Debian:
```
apt-get install make cmake pkg-config gkrellm libgtk2.0-dev libsystemd-dev gcc g++
```

Run `cmake .` and then `make` in gkrellm-sdnotify's source code
directory. You can also use the *Dockerfile* present in this
repository for reference, or as a build environment.

Use `make install` to install it, or copy gkrellm-sdnotify.so to
``$HOME/.gkrellm2/plugins``.


## Configuration

Enable it in gkrellm's plugin configuration.

You can set up a delay in the plugin *Options* pane in gkrellm.
