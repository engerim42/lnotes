#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "data.h"

void drawviolin();
void drawline();
void preparenotes();
void drawbass();
void addbuttons(int bbstyle);
int buttonscreated = 0;
static gint configure_event(GtkWidget * widget, GdkEventConfigure * event);
static gint expose_event(GtkWidget * widget, GdkEventExpose * event);

GtkWidget *violin_pixmap, *violin_fixed;
GtkWidget *line_pixmap[10], *line_fixed[10];
GtkWidget *bass_pixmap, *bass_fixed;
GtkWidget *drawing_area;
GtkWidget *Form1, *child;
GtkWidget *Form1_fix;
GdkBitmap *mask;
GtkStyle *style;
GdkGC *gc;


GtkWidget *status_bar;
int currentclef = 0;
int currentnote = 0;
int nright, nwrong = 0;
typedef struct {
    GtkWidget *notebutton;
    GtkWidget *fixed;
    GtkWidget *pixmap1;
    int bass;
} noteinfostruct;

noteinfostruct noteinfo[23];

void randomnote()
{
    int j, chr, i;
    FILE *fp;

    fp = fopen("/dev/random", "rb");
    chr = fgetc(fp);
    fclose(fp);
    srand(time(NULL) + chr);
    currentnote = 1 + (int) (24.0 * rand() / (RAND_MAX + 1.0));
    currentclef = 1 + (int) (2.0 * rand() / (RAND_MAX + 1.0));
    currentclef--;
    currentnote--;
    addbuttons(currentclef);

    for (i = 0; i < 24; i++) {
	gtk_widget_hide(noteinfo[i].fixed);
	gtk_widget_hide(noteinfo[i].pixmap1);
    }
    gtk_widget_show(noteinfo[currentnote].fixed);
    gtk_widget_show(noteinfo[currentnote].pixmap1);
    for (i = 0; i < 9; i++) {
	gtk_widget_hide(line_fixed[i]);
	gtk_widget_hide(line_pixmap[i]);

    }


    switch (currentnote) {

    case 0:
	gtk_widget_show(line_fixed[5]);
	gtk_widget_show(line_pixmap[5]);
    case 1:
    case 2:
	gtk_widget_show(line_fixed[6]);
	gtk_widget_show(line_pixmap[6]);
    case 3:
    case 4:
	gtk_widget_show(line_fixed[7]);
	gtk_widget_show(line_pixmap[7]);
    case 5:
	gtk_widget_show(line_fixed[8]);
	gtk_widget_show(line_pixmap[8]);
	break;
    case 23:
    case 22:
	gtk_widget_show(line_fixed[2]);
	gtk_widget_show(line_pixmap[2]);
    case 21:
    case 20:
	gtk_widget_show(line_fixed[1]);
	gtk_widget_show(line_pixmap[1]);
    case 19:
    case 18:
	gtk_widget_show(line_fixed[0]);
	gtk_widget_show(line_pixmap[0]);
    default:
	break;

    }


}

static GdkPixmap *pixmap = NULL;

static gint configure_event(GtkWidget * widget, GdkEventConfigure * event)
{
    int i;

    int offset = 40;

    if (pixmap)
	gdk_pixmap_unref(pixmap);

    pixmap = gdk_pixmap_new(widget->window,
			    widget->allocation.width,
			    widget->allocation.height, -1);
    gdk_draw_rectangle(pixmap,
		       widget->style->white_gc,
		       TRUE,
		       0, 0,
		       widget->allocation.width,
		       widget->allocation.height);



    for (i = 0; i < 5; i++)
	gdk_draw_line(pixmap, widget->style->black_gc, 10, 7 * i + offset,
		      200, 7 * i + offset);


    return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gint expose_event(GtkWidget * widget, GdkEventExpose * event)
{
    gdk_draw_pixmap(widget->window,
		    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y, event->area.width,
		    event->area.height);

    return FALSE;
}

/* Draw a rectangle on the screen */
static void draw_brush(GtkWidget * widget, gdouble x, gdouble y)
{
    GdkRectangle update_rect;

    update_rect.x = x - 5;
    update_rect.y = y - 5;
    update_rect.width = 10;
    update_rect.height = 10;
    gdk_draw_rectangle(pixmap,
		       widget->style->black_gc,
		       TRUE,
		       update_rect.x, update_rect.y,
		       update_rect.width, update_rect.height);
    gtk_widget_draw(widget, &update_rect);
}

static gint button_press_event(GtkWidget * widget, GdkEventButton * event)
{
    return TRUE;
}

static gint motion_notify_event(GtkWidget * widget, GdkEventMotion * event)
{
    int x, y;
    GdkModifierType state;

    if (event->is_hint)
	gdk_window_get_pointer(event->window, &x, &y, &state);
    else {
	x = event->x;
	y = event->y;
	state = event->state;
    }
    return TRUE;
}

void buttonclick(GtkWidget * widget, gpointer data)
{
    int i;
    char tempstr[255];
    for (i = 0; i < 24; i++) {
	if (noteinfo[i].notebutton == widget) {
	    if (currentnote == i) {
		nright++;
		randomnote();
		break;

	    } else {
		nwrong++;
		break;
	    }
	}
    }
    sprintf(tempstr, "Right: %i Wrong: %i", nright, nwrong);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), 1, tempstr);

}

void quit()
{
    gtk_exit(0);
} int main(int argc, char *argv[])
{



    int i;
    char str[20];
    char tempstr[255];


    gtk_init(&argc, &argv);

    Form1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    Form1_fix = gtk_fixed_new();
    gtk_widget_set_usize(Form1, 230, 250);

    gtk_widget_set_name(Form1_fix, "Noten");

    gtk_signal_connect(GTK_OBJECT(Form1), "destroy",
		       GTK_SIGNAL_FUNC(quit), NULL);

    /* Create the drawing area */

    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), 210, 120);
    gtk_widget_set_uposition(drawing_area, 10, 10);

    status_bar = gtk_statusbar_new();
    gtk_widget_set_uposition(status_bar, 10, 230);
    gtk_widget_set_usize(status_bar, 210, 20);
    gtk_container_add(GTK_CONTAINER(Form1_fix), status_bar);

    gtk_widget_show(drawing_area);
    gtk_widget_show(status_bar);

    gtk_statusbar_push(GTK_STATUSBAR(status_bar), 1, "Right: 0 Wrong: 0");


    /* Signals used to handle backing pixmap */

    gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event",
		       (GtkSignalFunc) expose_event, NULL);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "configure_event",
		       (GtkSignalFunc) configure_event, NULL);

    /* Event signals */


    gtk_signal_connect(GTK_OBJECT(drawing_area),
		       "motion_notify_event",
		       (GtkSignalFunc) motion_notify_event, NULL);
    gtk_signal_connect(GTK_OBJECT(drawing_area),
		       "button_press_event",
		       (GtkSignalFunc) button_press_event, NULL);

    gtk_widget_set_events(drawing_area, GDK_EXPOSURE_MASK
			  | GDK_LEAVE_NOTIFY_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_POINTER_MOTION_MASK
			  | GDK_POINTER_MOTION_HINT_MASK);

    gtk_container_add(GTK_CONTAINER(Form1), Form1_fix);

    gtk_container_add(GTK_CONTAINER(Form1_fix), drawing_area);

    gtk_widget_show(Form1_fix);
    gtk_widget_show(Form1);

    style = gtk_widget_get_default_style();
    gc = style->black_gc;


    drawviolin();
    drawbass();
    drawline();
    addbuttons(1);

    preparenotes();

    gtk_widget_show(drawing_area);

    for (i = 0; i < 24; i++) {
	gtk_widget_show(noteinfo[i].notebutton);
    }
    randomnote();
    gtk_window_set_title(GTK_WINDOW(Form1), "Notes");
    gtk_main();

    return 0;
}

void drawviolin()
{
    GdkPixmap *gdk_pixmap;
    GdkBitmap *mask;

    gdk_pixmap = gdk_pixmap_create_from_xpm_d(Form1->window, &mask,
					      &style->bg
					      [GTK_STATE_NORMAL],
					      violin_xpm);
    violin_pixmap = gtk_pixmap_new(gdk_pixmap, mask);

    violin_fixed = gtk_fixed_new();
    gtk_widget_set_usize(violin_fixed, 26, 49);
    gtk_fixed_put(GTK_FIXED(violin_fixed), violin_pixmap, 0, 0);
    gtk_container_add(GTK_CONTAINER(Form1_fix), violin_fixed);
    gtk_widget_set_uposition(violin_fixed, 20, 40);

} void preparenotes()
{
    GdkPixmap *gdk_pixmap;
    GdkBitmap *mask;
    int i, start, useline, pos;
    pos = 0;
    useline = 0;
    for (i = 11; i >= 0; i--) {

	for (useline = 0; useline < 2; useline++) {
	    gdk_pixmap =
		gdk_pixmap_create_from_xpm_d(Form1->window, &mask,
					     &style->bg
					     [GTK_STATE_NORMAL],
					     (useline ==
					      0) ? note_line_xpm :
					     note_noline_xpm);
	    noteinfo[pos].pixmap1 = gtk_pixmap_new(gdk_pixmap, mask);

	    noteinfo[pos].fixed = gtk_fixed_new();
	    gtk_widget_set_usize(noteinfo[pos].fixed,
				 useline == 0 ? 17 : 11, 7);
	    gtk_fixed_put(GTK_FIXED(noteinfo[pos].fixed),
			  noteinfo[pos].pixmap1, 0, 0);
	    gtk_container_add(GTK_CONTAINER(Form1_fix),
			      noteinfo[pos].fixed);
	    if (useline == 0) {
		gtk_widget_set_uposition(noteinfo[pos].fixed, 60,
					 26 + (i * 7));
	    } else
		gtk_widget_set_uposition(noteinfo[pos].fixed, 63,
					 23 + (i * 7));

	    pos++;
	}

    }

}

void addbuttons(int bstyle)
{
    int b, i, l, c, k, xoffset, yoffset;
    char str[20];
    char tempstr[255];


    xoffset = 0;
    yoffset = 130;
    l = 10;

    if (bstyle == 1) {
	gtk_widget_show(violin_fixed);
	gtk_widget_show(violin_pixmap);
	gtk_widget_hide(bass_fixed);
	gtk_widget_hide(bass_pixmap);


	k = 40;
	c = 'D';
	b = 0;
    } else {
	gtk_widget_show(bass_fixed);
	gtk_widget_show(bass_pixmap);
	gtk_widget_hide(violin_fixed);
	gtk_widget_hide(violin_pixmap);


	k = 100;
	c = 'F';
	b = -2;
    }

    for (i = 0; i < 24; i++) {
	sprintf(str, "%c%i", c, b);
	if (buttonscreated == 0) {
	    noteinfo[i].notebutton = gtk_button_new_with_label(str);
	    gtk_widget_set_usize(noteinfo[i].notebutton, 30, 20);
	}
	gtk_widget_set_uposition(noteinfo[i].notebutton,
				 k + xoffset, l + yoffset);


	child = GTK_BIN(noteinfo[i].notebutton)->child;
	if (child)
	    if (GTK_IS_LABEL(child))
		gtk_label_set_text(GTK_LABEL(child), str);

	if (buttonscreated == 0) {
	    gtk_signal_connect_object(GTK_OBJECT
				      (noteinfo[i].notebutton),
				      "clicked",
				      GTK_SIGNAL_FUNC(buttonclick),
				      (gpointer) noteinfo[i].notebutton);


	    gtk_container_add(GTK_CONTAINER(Form1_fix),
			      noteinfo[i].notebutton);
	}
	k += 30;
	c++;
	if (c == 'H')
	    c = 'A';
	if (c == 'C') {
	    k = 10;
	    b++;
	    l += 20;
	}

    }
    buttonscreated = 1;
}

void drawbass()
{
    GdkPixmap *gdk_pixmap;
    GdkBitmap *mask;

    gdk_pixmap = gdk_pixmap_create_from_xpm_d(Form1->window, &mask,
					      &style->bg
					      [GTK_STATE_NORMAL],
					      bass_xpm);
    bass_pixmap = gtk_pixmap_new(gdk_pixmap, mask);

    bass_fixed = gtk_fixed_new();
    gtk_widget_set_usize(bass_fixed, 26, 35);
    gtk_fixed_put(GTK_FIXED(bass_fixed), bass_pixmap, 0, 0);
    gtk_container_add(GTK_CONTAINER(Form1_fix), bass_fixed);
    gtk_widget_set_uposition(bass_fixed, 20, 45);
} void drawline()
{
    GdkPixmap *gdk_pixmap;
    GdkBitmap *mask;
    int i, b, add = 0;

    gdk_pixmap = gdk_pixmap_create_from_xpm_d(Form1->window, &mask,
					      &style->bg
					      [GTK_STATE_NORMAL],
					      line_xpm);
    for (i = 0; i < 9; i++) {
	if (i <= 4)
	    add = 43;
	else
	    add = 141;
	line_pixmap[i] = gtk_pixmap_new(gdk_pixmap, mask);

	line_fixed[i] = gtk_fixed_new();
	gtk_widget_set_usize(line_fixed[i], 17, 1);
	gtk_fixed_put(GTK_FIXED(line_fixed[i]), line_pixmap[i], 0, 0);
	gtk_container_add(GTK_CONTAINER(Form1_fix), line_fixed[i]);
	gtk_widget_set_uposition(line_fixed[i], 60, add - b);
	b += 7;
    }
}
