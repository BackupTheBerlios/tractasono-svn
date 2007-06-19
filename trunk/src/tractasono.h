// Globale Includes
#include <gtk/gtk.h>
#include <glade/glade.h>

// Globale Variablen
GladeXML *xml;
GtkWidget *mainwindow;
GtkWidget *vbox_placeholder;
GtkWidget *vbox_keyboard;
GtkWidget *vbox_tractasono;

GtkEntry *actual_entry;


typedef struct {
	GtkWidget *music;
	GtkWidget *radio;
	GtkWidget *import;
	GtkWidget *settings;
	GtkWidget *fullscreen;
	GtkWidget *disc;
} WindowModule;

WindowModule module;
