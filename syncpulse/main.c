/**
 * Copyright 2023, SyncPulse
 *     Author: Yoon SeungYong <forcom@forcom.kr>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 **/

/*
pipeline for rtsp server test
gst-launch-1.0 uridecodebin uri=rtspt://127.0.0.1:8554/test ! autovideosink
*/

#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include "rtsp-server.h"
#include "composition.h"

static gboolean my_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
  GMainLoop* loop = (GMainLoop*)data;

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;
      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
      /* end-of-stream */
      g_main_loop_quit (loop);
      break;
    default:
      /* unhandled message */
      break;
  }
  /* we want to be notified again the next time there is a message
  * on the bus, so returning TRUE (FALSE means we want to stop watching
  * for messages on the bus and our callback should not be called again)
  */
  return TRUE;
}

int
syncpulse_main (int argc, char *argv[])
{
  GMainLoop *loop;
  CustomData *data;
  GstBus *bus;

  if(run_rtsp_server("8554") != 0)
  {
    g_print ("failed to run rtsp server\n");
    return -1;
  }

  loop = g_main_loop_new (NULL, FALSE);

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  gchar * media_uri = "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm";

  /* Build the pipeline */
  data = build_pipeline (media_uri, media_uri, media_uri, media_uri);

  /* TODO: use rtsp uri */
  // data = build_pipeline ("rtsp://localhost:8554/test", "rtsp://localhost:8555/test", "rtsp://localhost:8556/test", "rtsp://localhost:8557/test");

  /* Start playing */
  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (data->pipeline);
  gst_bus_add_watch (bus, my_bus_callback, loop);
  gst_object_unref (bus);
  
  g_main_loop_run (loop);
  
  gst_object_unref (data->source1->compositor_pad);
  gst_object_unref (data->source2->compositor_pad);
  gst_object_unref (data->source3->compositor_pad);
  gst_object_unref (data->source4->compositor_pad);
  gst_element_set_state (data->pipeline, GST_STATE_NULL);
  gst_object_unref (data->pipeline);
  free(data);
  stop_rtsp_server();
  return 0;
}

int
main (int argc, char *argv[])
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
  return gst_macos_main (syncpulse_main, argc, argv, NULL);
#else
  return syncpulse_main (argc, argv);
#endif
}
