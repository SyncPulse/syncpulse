#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__
#include <gst/gst.h>

#define DEFAULT_RTSP_PORT "8554"

int run_rtsp_server (gchar * media_uri, char * rtsp_server_port);

#endif /*__RTSP_SERVER_H__*/
