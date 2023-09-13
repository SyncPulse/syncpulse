/**
 * Copyright 2023, SyncPulse
 *     Author: Yoon SeungYong <forcom@forcom.kr>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 **/

#ifndef __COMPOSITION_H__
#define __COMPOSITION_H__
#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _Size {
  gint width;
  gint height;
} Size;

typedef struct _Composition {
  GstElement *source;
  int xpos;
  int ypos;
  Size size;
  GstElement *compositor;
  GstPad *compositor_pad;
} Composition;

typedef struct _CustomData {
  GstElement *pipeline;
  Composition *source1;
  Composition *source2;
  Composition *source3;
  Composition *source4;
  GstElement *compositor;
  GstElement *sink;
} CustomData;

static Composition *
new_composition (GstElement *source, int xpos, int ypos, GstElement *compositor) {
  Composition *comp = malloc(sizeof(Composition));
  memset(comp, 0, sizeof(Composition));
  comp->source = source;
  comp->xpos = xpos;
  comp->ypos = ypos;
  comp->compositor = compositor;
  return comp;
}

CustomData * build_pipeline (gchar * source1_uri, gchar * source2_uri, gchar * source3_uri, gchar * source4_uri);

static void pad_added_handler (GstElement *src, GstPad *new_pad, Composition *composition);

#endif /*__COMPOSITION_H__*/
