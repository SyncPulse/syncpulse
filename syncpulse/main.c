/**
 * Copyright 2023, SyncPulse
 *     Author: Yoon SeungYong <forcom@forcom.kr>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 **/

#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include "rtsp-server.h"
#include "composition.h"

int
syncpulse_main (int argc, char *argv[])
{
  CustomData *data;
  GstBus *bus;
  GstMessage *msg;

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
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    g_error ("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
        "variable set for more details.");
  }

  /* Free resources */
  if (msg != NULL) {
    gst_message_unref (msg);
  }
  gst_object_unref (bus);
  gst_object_unref (data->source1->compositor_pad);
  gst_object_unref (data->source2->compositor_pad);
  gst_object_unref (data->source3->compositor_pad);
  gst_object_unref (data->source4->compositor_pad);
  gst_element_set_state (data->pipeline, GST_STATE_NULL);
  gst_object_unref (data->pipeline);
  free(data);
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
