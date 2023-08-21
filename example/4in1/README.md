# syncpulse example: 4 in 1

## How to compile

```
gcc main.c test-merge.c test-rtsp.c -o main `pkg-config --cflags --libs gstreamer-1.0 gstreamer-rtsp-server-1.0`
```

## How to run

```
./main
```
