/**
 * Copyright 2023, SyncPulse
 *     Author: Yoon SeungYong <forcom@forcom.kr>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 **/

/*
1. pipeline for rtsp server test
gst-launch-1.0 uridecodebin uri=rtspt://127.0.0.1:8554/test ! autovideosink
2. list arguments
syncpulse.exe(./syncpulse) -h
3. argument usage
syncpulse.exe(./syncpulse) -r 8554 -m /test -b 8192 -u https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm -a 127.0.0.1 -p 5000
*/

#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#include "rtsp-server.h"
#include "composition.h"
#include "argument.h"

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
  GOptionContext *optctx;
  GError *error = NULL;

  Argument *argument = g_new0(Argument, 1);

  argument->rtsp_port = (char *) DEFAULT_RTSP_PORT;
  argument->rtsp_mount = (char *) DEFAULT_MOUNT;
  argument->bitrate = atoi((char *) DEFAULT_BITRATE);
  argument->media_uri = (char *) DEFAULT_MEDIA_URI;
  argument->udp_bind_address = (char *) DEFAULT_UDP_BIND_ADDRESS;
  argument->udp_port = atoi((char *) DEFAULT_UDP_PORT);

  GOptionEntry entries[] = {
    {"rtsp-port", 'r', 0, G_OPTION_ARG_STRING, &argument->rtsp_port,
        "RTSP Server Port to listen on (default: " DEFAULT_RTSP_PORT ")", "PORT"},
    {"rtsp-mount", 'm', 0, G_OPTION_ARG_STRING, &argument->rtsp_mount,
        "RTSP Server Mount Point (default: " DEFAULT_MOUNT ")", "MOUNT"},
    {"bitrate", 'b', 0, G_OPTION_ARG_INT, &argument->bitrate,
        "Encoding Bitrate in kbit/sec (default: " DEFAULT_BITRATE ")", "BITRATE"},
    {"media-uri", 'u', 0, G_OPTION_ARG_STRING, &argument->media_uri,
        "Media uri to connect (default: " DEFAULT_MEDIA_URI ")", "URI"},
    {"udp-bind-address", 'a', 0, G_OPTION_ARG_STRING, &argument->udp_bind_address,
        "UDP bind address (default: " DEFAULT_UDP_BIND_ADDRESS ")", "ADDRESS"},
    {"udp-port", 'p', 0, G_OPTION_ARG_INT, &argument->udp_port,
        "UDP Port (default: " DEFAULT_UDP_PORT ")", "PORT"},
    {NULL}
  };

  optctx = g_option_context_new ("[ADDITIONAL ARGUMENTS]");
  g_option_context_add_main_entries (optctx, entries, NULL);
  g_option_context_add_group (optctx, gst_init_get_option_group ());
  if (!g_option_context_parse (optctx, &argc, &argv, &error)) {
    g_printerr ("Error parsing options: %s\n", error->message);
    g_option_context_free (optctx);
    g_clear_error (&error);
    return -1;
  }
  g_option_context_free (optctx);

  if(run_rtsp_server(argument) != 0)
  {
    g_print ("failed to run rtsp server\n");
    return -1;
  }

  loop = g_main_loop_new (NULL, FALSE);

  /* Build the pipeline */
  data = build_pipeline (argument);

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
  g_free(argument);
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
