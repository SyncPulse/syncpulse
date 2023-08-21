#ifndef __TEST_RTSP_H__
#define __TEST_RTSP_H__
#include <gst/gst.h>

#define DEFAULT_RTSP_PORT "8554"

int run_rtsp_server (gchar * media_uri, char * rtsp_server_port);

#endif /*__TEST_RTSP_H__*/