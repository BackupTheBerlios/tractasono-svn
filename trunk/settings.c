#include "settings.h"

#include <glib.h>
#include <gconf/gconf-client.h>

/* Update the GConf key when the user toggles the check button. */
/*static void button_toggled_cb(GtkWidget *button, gpointer data)
{
	GConfClient *client;
	gboolean     value;
	client = gconf_client_get_default ();
	value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
	gconf_client_set_bool (client, KEY, value, NULL);
}*/

/* This is called when our key is changed. */
static void gconf_notify_func(GConfClient *client, guint cnxn_id, GConfEntry  *entry, gpointer user_data)
{
	//GtkWidget  *check_button = GTK_WIDGET (user_data);
	GConfValue *value;
	gboolean    checked;

	value = gconf_entry_get_value (entry);
	checked = gconf_value_get_bool (value);

	g_print("GConf Event: %i\n", checked);

	/* Update the check button accordingly. */
	/*gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button), checked);*/
}

void init_settings()
{
	GConfClient *client;
	gboolean value;

	client = gconf_client_get_default ();

	
	value = gconf_client_get_bool (client, KEY, NULL);

	/* Add our directory to the list of directories the GConfClient will
	 * watch.
	 */
	gconf_client_add_dir (client, PATH, GCONF_CLIENT_PRELOAD_NONE, NULL);

	/* Listen to changes to our key. */
	gconf_client_notify_add (client, KEY, gconf_notify_func, NULL, NULL, NULL);

	gconf_client_set_bool (client, KEY, TRUE, NULL);
}
