/* TODO: add MPL license here */

#include <gst/gst.h>

#include "test-merge.h"

// build pipeline
CustomData *
build_pipeline (gchar * source1_uri, gchar * source2_uri, gchar * source3_uri, gchar * source4_uri) {
  CustomData * data = malloc(sizeof(CustomData));

  data->compositor = gst_element_factory_make ("compositor", "compositor");
  data->source1 = new_composition(
    gst_element_factory_make ("uridecodebin", "source1"),
    0, 0,
    data->compositor
  );
  data->source2 = new_composition(
    gst_element_factory_make ("uridecodebin", "source2"),
    1, 0,
    data->compositor
  );
  data->source3 = new_composition(
    gst_element_factory_make ("uridecodebin", "source3"),
    0, 1,
    data->compositor
  );
  data->source4 = new_composition(
    gst_element_factory_make ("uridecodebin", "source4"),
    1, 1,
    data->compositor
  );
  data->sink = gst_element_factory_make ("autovideosink", "sink");

  data->pipeline = gst_pipeline_new ("test-pipeline");

  if (!data->pipeline ||
      !data->source1->source || !data->source2->source || !data->source3->source || !data->source4->source ||
      !data->compositor ||
      !data->sink) {
    g_printerr ("Not all elements could be created.\n");
    return data;
  }

  g_object_set (data->source1->source, "uri", source1_uri, NULL);
  g_object_set (data->source2->source, "uri", source2_uri, NULL);
  g_object_set (data->source3->source, "uri", source3_uri, NULL);
  g_object_set (data->source4->source, "uri", source4_uri, NULL);

  /* Link sources to compositor */
  g_signal_connect (data->source1->source, "pad-added", G_CALLBACK(pad_added_handler), data->source1);
  g_signal_connect (data->source2->source, "pad-added", G_CALLBACK(pad_added_handler), data->source2);
  g_signal_connect (data->source3->source, "pad-added", G_CALLBACK(pad_added_handler), data->source3);
  g_signal_connect (data->source4->source, "pad-added", G_CALLBACK(pad_added_handler), data->source4);

  /* TODO: insert videoresize pipeline between source and compositor */
  gst_bin_add_many (GST_BIN (data->pipeline), data->source1->source, data->source2->source, data->source3->source, data->source4->source, data->compositor, data->sink, NULL);

  /* Link compositor to sink */
  gst_element_link (data->compositor, data->sink);

  return data;
}

/* This function will be called by the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *new_pad, Composition *composition) {
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;
  GstPadLinkReturn ret;
  gint width = 0, height = 0;

  g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

  /* Check new pad's type */
  new_pad_caps = gst_pad_query_caps (new_pad, NULL);
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);
  if (!g_str_has_prefix (new_pad_type, "video/x-raw")) {
    g_print ("A new pad '%s' from '%s' has type '%s' which is not raw video. Ignoring.\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src), new_pad_type);
    goto exit;
  } else {
    g_print ("A new pad '%s' from '%s' has type '%s' which is raw video.\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src), new_pad_type);
  }

  /* TODO: get width and height from caps */
  composition->size.width = 854;
  composition->size.height = 480;

  /* If our converter is already linked, we have nothing to do here */
  composition->compositor_pad = gst_element_request_pad_simple (composition->compositor, "sink_%u");
  if (gst_pad_is_linked (composition->compositor_pad)) {
    g_print ("We are already linked. Ignoring.\n");
    goto exit;
  }

  g_object_set (composition->compositor_pad, "xpos", composition->xpos * composition->size.width, NULL);
  g_object_set (composition->compositor_pad, "ypos", composition->ypos * composition->size.height, NULL);

  /* Link the uridecodebin pad to compositor */
  ret = gst_pad_link (new_pad, composition->compositor_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_print ("Type is '%s' but link failed.\n", new_pad_type);
  } else {
    g_print ("A new pad '%s' from '%s' link succeeded (type '%s').\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src), new_pad_type);
  }


exit:
  /* Unreference the new pad's caps, if we got them */
  if (new_pad_caps != NULL) {
    gst_caps_unref (new_pad_caps);
  }
}
