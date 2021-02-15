#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>

int
main (int   argc,
      char *argv[])
{
  GMainLoop *loop;

  GstElement *pipeline, *src, *vscale, *filter, *vconv, *fish, *sink;
  GstCaps *filtercaps;

  /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  /* Create gstreamer elements */
  pipeline = gst_element_factory_make("pipeline",      "pipeline");
  src      = gst_element_factory_make("v4l2src",       "src");
  vscale   = gst_element_factory_make("videoscale",    "vscale");
  filter   = gst_element_factory_make("capsfilter",    "filter");
  vconv    = gst_element_factory_make("videoconvert",  "vconv");
  fish     = gst_element_factory_make("fisheye",       "fish");
  sink     = gst_element_factory_make("ximagesink",    "sink");

  if (!pipeline || !src || !vscale || !filter || !vconv || !fish || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }

  /* Set up the pipeline */

  /* we set the input filename to the source element */
  g_object_set (G_OBJECT (src), "device", "/dev/video0", "io-mode", 0, NULL);

  filtercaps = gst_caps_new_simple(
    "video/x-raw",
    "width",  G_TYPE_INT, 640,
    "height", G_TYPE_INT, 480,
    NULL);
  g_object_set (G_OBJECT(filter),"caps",filtercaps, NULL);

  g_object_set (G_OBJECT(sink),
                "sync",FALSE,
                NULL);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (pipeline),
                    src, vscale, filter, vconv, fish, sink, NULL);

  gst_element_link_many (src, vscale, filter, vconv, fish, sink, NULL);

  /* Set the pipeline to "playing" state*/
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);

  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  g_main_loop_unref (loop);

  return 0;
}
