/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/gst.h>

#include <gst/rtsp-server/rtsp-server.h>

#include "rtsp-server.h"

static GstRTSPServer *server;
static guint server_id;

int
run_rtsp_server (char * rtsp_server_port)
{
  GstRTSPMountPoints *mounts;
  GstRTSPMediaFactory *factory;
  
  /* create a server instance */
  server = gst_rtsp_server_new ();
  g_object_set (server, "service", rtsp_server_port, NULL);

  /* get the mount points for this server, every server has a default object
   * that be used to map uri mount points to media factories */
  mounts = gst_rtsp_server_get_mount_points (server);

  /* make a URI media factory for a test stream. */
  factory = gst_rtsp_media_factory_new ();
  gst_rtsp_media_factory_set_launch (factory, "( "
  "udpsrc port=5000 "
  "caps = \"application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)H264\" ! "
  "rtph264depay ! rtph264pay name=pay0 " ")");
  gst_rtsp_media_factory_set_shared (factory, TRUE);

 /* attach the test factory to the mount url */
  gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

  /* don't need the ref to the mapper anymore */
  g_object_unref (mounts);

  /* attach the server to the default maincontext */
  server_id = gst_rtsp_server_attach (server, NULL);
  if (server_id == 0)
    goto failed;

  /* start serving */
  g_print ("stream ready at rtsp://localhost:%s/test\n", rtsp_server_port);
  return 0;

  /* ERRORS */
failed:
  {
    g_print ("failed to attach the server\n");
    return -1;
  }
}

void stop_rtsp_server()
{
  g_source_remove (server_id);
  GstRTSPMountPoints *mounts;
  mounts = gst_rtsp_server_get_mount_points(server);
  gst_rtsp_mount_points_remove_factory (mounts, "/test");
  g_object_unref (mounts);

  int serv_ref_cnt = GST_OBJECT_REFCOUNT_VALUE(server);
  int i;
  for (i = 0; i < serv_ref_cnt; i++)
  {
    g_object_unref(server);
  }
}
