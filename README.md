# SyncPulse
-----------

SyncPulse is a compositor of multiple video sources.

# Getting started
-----------------

SyncPulse requires meson, ninja and GStreamer.

## Install meson and ninja

```
$ pip3 install --user meson
```


## Build SyncPulse

You can get SyncPulse executable built running:

```
meson setup builddir
meson compile -C builddir
```

This will automatically create the ```builddir``` directory and build everything inside it.


You can install SyncPulse running:
```
meson install -C builddir
```
