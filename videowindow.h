#ifndef PIROVERC_VIDEOWINDOW_H
#define PIROVERC_VIDEOWINDOW_H

class VideoWindow
{
    public:
        VideoWindow();
        ~VideoWindow();

        gulong get_embed_xid(void);

    private:
        GtkWidget *make_toolbar(void);

        GtkWidget *video_window, *window;
};

#endif // PIROVERC_WINDOW_H