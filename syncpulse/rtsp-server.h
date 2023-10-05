#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__
#include <gst/gst.h>
#include "argument.h"

int run_rtsp_server (Argument *argument);
void stop_rtsp_server();

#endif /*__RTSP_SERVER_H__*/
