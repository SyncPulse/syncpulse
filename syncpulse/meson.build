gst_deps = [
  dependency('gstreamer-1.0'),
  dependency('gstreamer-rtsp-server-1.0'),
]

executable('syncpulse',
           'main.c',
           'rtsp-server.c',
           'composition.c',
           dependencies: gst_deps,
           install : true)
