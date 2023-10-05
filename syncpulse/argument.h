/**
 * Copyright 2023, SyncPulse
 *     Author: Nam IlWoo <ilwoonam75@gmail.com>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 **/

#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#define DEFAULT_RTSP_PORT "8554"
#define DEFAULT_MOUNT "/test"
#define DEFAULT_BITRATE "8192"
#define DEFAULT_MEDIA_URI "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm"
#define DEFAULT_UDP_BIND_ADDRESS "127.0.0.1"
#define DEFAULT_UDP_PORT "5000"

typedef struct _Argument {
    gchar *rtsp_port;
    gchar *rtsp_mount;
    gint bitrate;
    gchar *media_uri;
    gchar *udp_bind_address;
    gint udp_port;
} Argument;

#endif /*__ARGUMENT_H__*/