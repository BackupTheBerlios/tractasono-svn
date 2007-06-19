#include "keyboard.h"
#include "tractasono.h"



// Zeige das Oncreen Keyboard an
void keyboard_show(gboolean show)
{
	if (show) {
		// Keyboard anzeigen
		g_print("Keyboard wird angezeigt!\n");

		gboolean visible;
		g_object_get(vbox_keyboard, "visible", &visible, NULL);
		if (!visible) {	
			gtk_widget_show(vbox_keyboard);
		}
		
	} else {
		// Keyboard ausblenden
		g_print("Keyboard wird ausgeblendet!\n");
		gtk_widget_hide(vbox_keyboard);
	}
}



void add_text_to_entry(const GString *text)
{
	const gchar *origtext = NULL;
	GString *newtext = NULL;

	origtext = gtk_entry_get_text(actual_entry);
	newtext = g_string_new(origtext);
	g_string_append(newtext, text->str);
	
	gtk_entry_set_text(actual_entry, newtext->str);

	gtk_widget_grab_focus(GTK_WIDGET(actual_entry));
}


// Tastatur Tastenhandler
void on_key_clicked(GtkButton *button, gpointer user_data)
{
	GString tastenlabel;
	
	g_object_get(G_OBJECT(button), "label", &tastenlabel, NULL);
	add_text_to_entry(&tastenlabel);
}




gboolean on_entry_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Eingabefeld hat Fokus bekommen!\n");

	actual_entry = GTK_ENTRY(widget);
	keyboard_show(TRUE);

	return(FALSE);
}



// Tastatur Spezial Tastenhandler
void on_specialkey_clicked(GtkButton *button, gpointer user_data)
{
	gchararray name;

	g_object_get(G_OBJECT(button), "name", &name, NULL);

	
	if (g_strcasecmp (name, "key_enter") == 0) {
		g_print("space\n");
		keyboard_show(FALSE);
		return;
	}

	if (g_strcasecmp (name, "key_esc") == 0) {
		g_print("ESC\n");
		keyboard_show(FALSE);
		return;
	}

	if (g_strcasecmp (name, "key_back") == 0) {
		g_print("Back\n");
		GString *temptext = NULL;
		temptext = g_string_new(gtk_entry_get_text(actual_entry));
		g_string_truncate(temptext, temptext->len -1);
		add_text_to_entry(temptext);
		return;
	}

	if (g_strcasecmp (name, "key_caps") == 0) {
		g_print("caps\n");
		return;
	}

	if (g_strcasecmp (name, "key_lshift") == 0) {
		g_print("shift\n");
		return;
	}

	if (g_strcasecmp (name, "key_rshift") == 0) {
		g_print("shift\n");
		return;
	}

	if (g_strcasecmp (name, "key_space") == 0) {
		g_print("space\n");
		GString *text = g_string_new(" ");
		add_text_to_entry(text);
		return;
	}
}


