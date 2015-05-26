#include <glib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include "control.h"

#include "videowindow.h"

static void toggle_headlights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_headlights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

static void toggle_taillights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_taillights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

static void toggle_hazardlights (GtkToggleToolButton *toggletoolbutton, gpointer user_data)
{
    control_set_hazardlights(gtk_toggle_tool_button_get_active(toggletoolbutton));
}

static void window_closed (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  gtk_widget_hide (widget);
  gtk_main_quit ();
}

VideoWindow::VideoWindow()
{
    GtkWidget *vbox;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (window_closed), NULL);

    gtk_window_set_default_size (GTK_WINDOW (window), 1000, 600);
    gtk_window_set_title (GTK_WINDOW (window), "PiRover Control");

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    gtk_box_pack_start(GTK_BOX(vbox), make_toolbar(), FALSE, FALSE, 5);

    video_window = gtk_drawing_area_new ();
    gtk_box_pack_start(GTK_BOX(vbox), video_window, TRUE, TRUE, 0);

    gtk_widget_show_all (window);
}

VideoWindow::~VideoWindow()
{
    ;
}

gulong VideoWindow::get_embed_xid(void)
{
    return GDK_WINDOW_XID (gtk_widget_get_window (video_window));
}

GtkWidget *VideoWindow::make_toolbar(void)
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

