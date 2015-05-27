/* main.c -- piroverd
 *
 * Copyright (C) 2015 Alistair Buxton <a.j.buxton@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modi$
 * it under the terms of the GNU General Public License as published $
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses$
 */

#define GDK_VERSION_MIN_REQUIRED (GDK_VERSION_3_0)

#include <glib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

#include "videowindow.h"
#include "control.h"
#include "net.h"


int main (int argc, char **argv)
{
    GstElement *pipeline, *sink;
    gulong embed_xid;
    GstStateChangeReturn sret;
    GError *err = NULL;

    gst_init (&argc, &argv);
    gtk_init (&argc, &argv);

    /* prepare the pipeline */

    pipeline = gst_parse_launch("rtspsrc location=\"rtsp://10.24.42.1:8554/test\" ! rtph264depay ! avdec_h264 ! xvimagesink sync=false name=sink", &err);
    //pipeline = gst_parse_launch("videotestsrc ! xvimagesink name=sink", &err);
    g_assert(err == NULL);

    /* prepare the ui */

    VideoWindow *w = new VideoWindow();

    sink = gst_bin_get_by_name_recurse_up (GST_BIN (pipeline), "sink");
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), w->get_embed_xid());
    g_object_unref(sink);

    /* run the pipeline */

    sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE)
        gst_element_set_state (pipeline, GST_STATE_NULL);

    /* Start input and networking */

    control_start();
    net_start();

    gtk_main ();

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    return 0;
}
