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
#include <gst/rtsp-server/rtsp-media-factory-uri.h>

#include "test-rtsp.h"

static gboolean
timeout (GstRTSPServer * server)
{
  GstRTSPSessionPool *pool;

  pool = gst_rtsp_server_get_session_pool (server);
  gst_rtsp_session_pool_cleanup (pool);
  g_object_unref (pool);

  return TRUE;
}

int
run_rtsp_server (gchar * media_uri, char * rtsp_server_port)
{
  GMainLoop *loop;
  GstRTSPServer *server;
  GstRTSPMountPoints *mounts;
  GstRTSPMediaFactoryURI *factory;
  GOptionContext *optctx;
  GError *error = NULL;
  gchar *uri;

  loop = g_main_loop_new (NULL, FALSE);

  /* create a server instance */
  server = gst_rtsp_server_new ();
  g_object_set (server, "service", rtsp_server_port, NULL);

  /* get the mount points for this server, every server has a default object
   * that be used to map uri mount points to media factories */
  mounts = gst_rtsp_server_get_mount_points (server);

  /* make a URI media factory for a test stream. */
  factory = gst_rtsp_media_factory_uri_new ();

  /* when using GStreamer as a client, one can use the gst payloader, which is
   * more efficient when there is no payloader for the compressed format */
  /* g_object_set (factory, "use-gstpay", TRUE, NULL); */

  /* check if URI is valid, otherwise convert filename to URI if it's a file */
  if (gst_uri_is_valid (media_uri)) {
    uri = g_strdup (media_uri);
  } else if (g_file_test (media_uri, G_FILE_TEST_EXISTS)) {
    uri = gst_filename_to_uri (media_uri, NULL);
  } else {
    g_printerr ("Unrecognized uri argument '%s'.\n", media_uri);
    return -1;
  }

  gst_rtsp_media_factory_uri_set_uri (factory, uri);
  g_free (uri);

  /* if you want multiple clients to see the same video, set the shared property
   * to TRUE */
  /* gst_rtsp_media_factory_set_shared ( GST_RTSP_MEDIA_FACTORY (factory), TRUE); */

  /* attach the test factory to the /test url */
  gst_rtsp_mount_points_add_factory (mounts, "/test",
      GST_RTSP_MEDIA_FACTORY (factory));

  /* don't need the ref to the mapper anymore */
  g_object_unref (mounts);

  /* attach the server to the default maincontext */
  if (gst_rtsp_server_attach (server, NULL) == 0)
    goto failed;

  /* do session cleanup every 2 seconds */
  g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

  /* start serving */
  g_print ("stream ready at rtsp://localhost:%s/test\n", rtsp_server_port);
  g_main_loop_run (loop);

  return 0;

  /* ERRORS */
failed:
  {
    g_print ("failed to attach the server\n");
    return -1;
  }
}
