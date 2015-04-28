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

#include <SDL.h>

#include "control.h"
#include "net.h"

static void window_closed (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  GstElement *pipeline = (GstElement *)user_data;

  gtk_widget_hide (widget);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gtk_main_quit ();
}

void toggle_headlights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_headlights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

void toggle_taillights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_taillights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

void toggle_hazardlights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_hazardlights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

GtkWidget *make_toolbar(void)
{
    GtkWidget *toolbar;

    GtkToolItem *headlights;
    GtkToolItem *taillights;
    GtkToolItem *hazardlights;

    toolbar = gtk_toolbar_new ();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

    headlights = gtk_toggle_tool_button_new();
    gtk_tool_button_set_label (GTK_TOOL_BUTTON (headlights), "Head Lights");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), headlights, -1);

    taillights = gtk_toggle_tool_button_new();
    gtk_tool_button_set_label (GTK_TOOL_BUTTON (taillights), "Tail Lights");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), taillights, -1);

    hazardlights = gtk_toggle_tool_button_new();
    gtk_tool_button_set_label (GTK_TOOL_BUTTON (hazardlights), "Hazard Lights");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), hazardlights, -1);



    g_signal_connect(G_OBJECT(headlights), "toggled",
        G_CALLBACK(toggle_headlights), NULL);

    g_signal_connect(G_OBJECT(taillights), "toggled",
        G_CALLBACK(toggle_taillights), NULL);

    g_signal_connect(G_OBJECT(hazardlights), "toggled",
        G_CALLBACK(toggle_hazardlights), NULL);

    return toolbar;
}

int main (int argc, char **argv)
{
    GdkWindow *video_window_xwindow;
    GtkWidget *window, *video_window, *vbox;
    GstElement *pipeline, *sink;
    gulong embed_xid;
    GstStateChangeReturn sret;
    GError *err = NULL;

    gst_init (&argc, &argv);
    gtk_init (&argc, &argv);

    /* prepare the pipeline */

    //pipeline = gst_parse_launch("rtspsrc location=\"rtsp://192.168.0.165:8554/test\" ! rtph264depay ! avdec_h264 ! xvimagesink sync=false name=sink", &err);
    pipeline = gst_parse_launch("videotestsrc ! xvimagesink name=sink", &err);
    g_assert(err == NULL);

    /* prepare the ui */

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "delete-event",
        G_CALLBACK (window_closed), (gpointer) pipeline);
    gtk_window_set_default_size (GTK_WINDOW (window), 1000, 600);
    gtk_window_set_title (GTK_WINDOW (window), "PiRover Control");

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    gtk_box_pack_start(GTK_BOX(vbox), make_toolbar(), FALSE, FALSE, 5);

    video_window = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (vbox), video_window);

    gtk_widget_show_all (window);

    video_window_xwindow = gtk_widget_get_window (video_window);
    embed_xid = GDK_WINDOW_XID (video_window_xwindow);
    sink = gst_bin_get_by_name_recurse_up (GST_BIN (pipeline), "sink");
    gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), embed_xid);
    g_object_unref(sink);

    /* run the pipeline */

    sret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE)
        gst_element_set_state (pipeline, GST_STATE_NULL);

    /* TODO: everything else */

    control_start();
    net_start();

    gtk_main ();

    gst_object_unref (pipeline);
    return 0;
}
