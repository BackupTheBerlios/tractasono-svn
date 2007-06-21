#include "player.h"
#include "interface.h"
#include "tractasono.h"


// GStreamer Zeit Format
GstFormat format = GST_FORMAT_TIME;

GstElement *pipeline;
GstElement *src;
GstElement *decoder;
GstElement *audiosink;

static GstState state;

typedef struct
{
  const gchar *name;
  const GstFormat format;
} seek_format;


/* seek timeout */
#define SEEK_TIMEOUT 40 * GST_MSECOND


GList *seekable_pads = NULL;
GList *rate_pads = NULL;
GList *seekable_elements = NULL;

gint64 position;

gboolean elem_seek = FALSE;
gboolean stats = FALSE;
gboolean accurate_seek = FALSE;
gboolean keyframe_seek = FALSE;
gboolean loop_seek = FALSE;
gboolean flush_seek = TRUE;
gboolean scrub = FALSE;


// Prototypen
GstElement* player_make_flac_pipeline(const gchar * location);
GstElement* player_make_stream_pipeline(const gchar *url);
static gboolean player_send_event(GstEvent *event);
void player_handle_tag_message(GstMessage *message);
static gboolean player_timer_event(GstElement *pipeline);



// Player initialisieren
void player_init(int *argc, char **argv[])
{
	g_print("Player initialisieren\n");
	gst_init (argc, argv);
}

static GstElement* gst_element_factory_make_or_warn(gchar * type,
													gchar * name)
{
  GstElement *element = gst_element_factory_make(type, name);

  if (!element) {
    g_warning("Failed to create element %s of type %s", name, type);
  }

  return element;
}


/*
	Gibt die Länge des Songs in Nanosekunden zurück
*/
gint64 player_get_song_duration_ns()
{
	//g_print("player_get_song_duration_ns()\n");
	
	gint64 duration;
	
	if (!gst_element_query_duration(pipeline, &format, &duration)) {
		g_print("Konnte duration nicht herausfinden!\n");
	}

	return duration;
}


/*
	Gibt die Position des Songs in Nanosekunden zurück
*/
gint64 player_get_song_position_ns()
{
	//g_print("player_get_song_position_ns()\n");
	
	gint64 position;
	
	if (!gst_element_query_position(pipeline, &format, &position)) {
		g_print("Konnte position nicht herausfinden!\n");
	}

	return position;
}

// Callback Behandlung
static gboolean player_bus_callback (GstBus *bus, GstMessage *message, gpointer data)
{
	//g_print ("GStreamer: Got %s message | %d | %d\n", GST_MESSAGE_TYPE_NAME(message), GST_MESSAGE_SRC(message), pipeline);
	if (GST_MESSAGE_SRC(message) != GST_OBJECT(pipeline)) {		
		return TRUE;
	}

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ERROR: {
			GError *err;
			gchar *debug;

			gst_message_parse_error (message, &err, &debug);
			g_print ("\tError: %s\n", err->message);
			g_error_free (err);
			g_free (debug);
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState oldstate, newstate, pending;
			
			gst_message_parse_state_changed(message, &oldstate, &newstate, &pending);
			g_print("\tStates: (old=%i, new=%i, pending=%i)\n", oldstate, newstate, pending);

			if (newstate == 4) {
				g_print("GStreamer is now playing!\n");
				
				// Watch hinzufügen
				g_timeout_add (1000, (GSourceFunc)player_timer_event, pipeline);
				
				interface_set_playing(TRUE);
				interface_set_song_duration(player_get_song_duration_ns());
			}
			
			if ((newstate == 3) && (oldstate == 4)) {
				g_print("GStreamer is now paused!\n");
				interface_set_playing(FALSE);
			}
			
		}
		case GST_MESSAGE_EOS: {
			//g_print ("\tEnd Of Stream\n");
			break;
		}
		case GST_MESSAGE_TAG: {
			/* Musik Tags */
			player_handle_tag_message(message);
			break;
		}
		default: {
			/* unhandled message */
			//g_print ("\tUnhandled Message %i\n", GST_MESSAGE_TYPE (message));
			break;
		}
	}

	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again) */
	return TRUE;
}

// Play
void player_set_play()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);
	
	state = GST_STATE_PLAYING;
}

// Gibt den aktuellen Player Status zurück
gboolean player_get_playing(void)
{
	GstState state;
	
	gst_element_get_state(GST_ELEMENT(pipeline), &state, NULL, GST_CLOCK_TIME_NONE);
	
	if (state != GST_STATE_PLAYING) {
		return FALSE;
	}
	return TRUE;
}

// Stop
void player_set_stop()
{
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_READY);
	
	state = GST_STATE_READY;
}

// Timer Funktion
static gboolean player_timer_event(GstElement *pipeline)
{	
	if (!player_get_playing()) {
		return FALSE;
	}
	
	if (interface_get_slidermove()) {
		return TRUE;
	}

	interface_set_song_position(player_get_song_position_ns());
	
	/* call me again */
	return TRUE;
}

// Spiele Testfile ab
void player_play_testfile()
{
	pipeline = player_make_flac_pipeline("data/test.flac");
	
	/* adds a watch for new message on our pipeline's message bus to
	* the default GLib main context, which is the main context that our
	* GLib main loop is attached to below */
	GstBus *bus;

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, player_bus_callback, NULL);
	gst_object_unref (bus);
}


// Play a stream
void player_play_stream(const gchar *url)
{
	pipeline = player_make_stream_pipeline(url);
	
	/* adds a watch for new message on our pipeline's message bus to
	* the default GLib main context, which is the main context that our
	* GLib main loop is attached to below */
	GstBus *bus;

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, player_bus_callback, NULL);
	gst_object_unref (bus);
}

// Seeking
void player_seek_to_position(gint seconds)
{
	g_print("Seeking to position: %i\n", seconds);

	GstEvent *event;
	seconds = seconds * GST_SECOND;
	
	if (gst_element_set_state(pipeline, GST_STATE_PAUSED) != GST_STATE_CHANGE_SUCCESS) {
		g_print("Fehler: Konnte state paused nicht setzen!\n");
	}
							 
	event = gst_event_new_seek(1.0,
							   GST_FORMAT_TIME,
							   GST_SEEK_FLAG_FLUSH,
							   GST_SEEK_TYPE_SET,
							   seconds,
							   GST_SEEK_TYPE_NONE,
							   0);

	if (gst_element_send_event (decoder, event)) {
		g_print("Seek Event konnte verschickt werden!\n");
	}
	
	if (gst_element_set_state(pipeline, GST_STATE_PLAYING) != GST_STATE_CHANGE_SUCCESS) {
		g_print("Fehler: Konnte state play nicht setzen!\n");
	}
}

GstElement* player_make_flac_pipeline(const gchar *location)
{
	GstPad *seekable;
	
	pipeline = gst_pipeline_new ("app");
	
	src = gst_element_factory_make_or_warn ("gnomevfssrc", "src");
	decoder = gst_element_factory_make_or_warn ("flacdec", "decoder");
	audiosink = gst_element_factory_make_or_warn ("alsasink", "sink");
	g_object_set (G_OBJECT (audiosink), "sync", FALSE, NULL);
	
	g_object_set (G_OBJECT (src), "location", location, NULL);
	
	gst_bin_add (GST_BIN (pipeline), src);
	gst_bin_add (GST_BIN (pipeline), decoder);
	gst_bin_add (GST_BIN (pipeline), audiosink);
	
	gst_element_link (src, decoder);
	gst_element_link (decoder, audiosink);
	
	seekable = gst_element_get_pad (decoder, "src");
	seekable_pads = g_list_prepend (seekable_pads, seekable);
	rate_pads = g_list_prepend (rate_pads, seekable);
	rate_pads = g_list_prepend (rate_pads, gst_element_get_pad (decoder, "sink"));
	
	return pipeline;
}

// Setze eine Pipeline für einen Stream auf
GstElement* player_make_stream_pipeline(const gchar *url)
{
	GstElement *play;
	
	play = gst_element_factory_make ("playbin", "play");
	g_object_set (G_OBJECT (play), "uri", url, NULL);
	gst_element_set_state (play, GST_STATE_PLAYING);
	
	return play;
}


static void player_do_seek(GtkWidget *widget)
{
	gint64 real;
	gint64 duration;
	gboolean res = FALSE;
	GstEvent *s_event;
	GstSeekFlags flags;
	
	
	duration = player_get_song_duration_ns();
	
	g_print("Duration: %lli \n", duration);
	
	gint64 pos = gtk_range_get_value(GTK_RANGE (widget));
	g_print("Position: %lli \n", pos);
	
	real = gtk_range_get_value(GTK_RANGE (widget)) * duration / 100;
	
	g_print("Real range: %lli \n", real);
	
	flags = 0;
	if (flush_seek) {
		flags |= GST_SEEK_FLAG_FLUSH;
	}
	if (accurate_seek) {
		flags |= GST_SEEK_FLAG_ACCURATE;
	}
	if (keyframe_seek) {
		flags |= GST_SEEK_FLAG_KEY_UNIT;
	}
	if (loop_seek) {
		flags |= GST_SEEK_FLAG_SEGMENT;
	}
	
	//real = 30000000000;

	if (real >= duration) {
		real = 0;
	}
	
	s_event = gst_event_new_seek (1.0, GST_FORMAT_TIME, flags,
								  GST_SEEK_TYPE_SET, real,
								  GST_SEEK_TYPE_NONE, 0);
	
	g_print("seek to %" GST_TIME_FORMAT "\n", GST_TIME_ARGS (real));							  
	//GST_DEBUG ("seek to %" GST_TIME_FORMAT, GST_TIME_ARGS (real));
	
	res = player_send_event (s_event);
	
	if (res) {
		if (flush_seek) {
			gst_pipeline_set_new_stream_time (GST_PIPELINE (pipeline), 0);
			gst_element_get_state (GST_ELEMENT (pipeline), NULL, NULL, SEEK_TIMEOUT);
		}
	} else {
		g_print ("seek failed\n");
	}
}



static gboolean player_send_event(GstEvent *event)
{
	gboolean res = FALSE;
	
	gst_event_ref(event);
	res = gst_element_send_event(pipeline, event);
	gst_event_unref(event);
	
	return res;
}

gboolean player_start_seek(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event->type != GDK_BUTTON_PRESS) {
		return FALSE;
	}
	
	if (state == GST_STATE_PLAYING && flush_seek) {
		g_print("start scrub seek, PAUSE\n");
		gst_element_set_state(pipeline, GST_STATE_PAUSED);
	}
	
	return FALSE;
}

gboolean player_stop_seek (GtkWidget * widget, gpointer user_data)
{	
	g_print("do final seek\n");
	
	player_do_seek(widget);
	
	if (state == GST_STATE_PLAYING) {
		g_print("stop scrub seek, PLAYING\n");
		gst_element_set_state (pipeline, GST_STATE_PLAYING);
	}
	
	return FALSE;
}


// Konvertiert Nanosekunden in Sekunden
gint64 ns_to_seconds(gint64 ns)
{	
	gint64 seconds;
	
	seconds = ns / GST_SECOND;
	
	return seconds;
}


// Nanosekunden schön formatiert in Sekunden und Millisekunden aus
gchar* ns_formatted(gint64 ns)
{	
	gint64 seconds;
	
	seconds = ns_to_seconds(ns);
	
	return g_strdup_printf("%02lli:%02lli", seconds/60, seconds%60);
}

// Formatiert die Scrollbalken Anzeige auf Minuten:Sekunden (00:00)
gchar* on_range_song_format_value(GtkScale * scale, gdouble value)
{
	g_print("on_range_song_format_value()\n");
	
	gint64 duration;
	
	if (pipeline) {
		duration = player_get_song_duration_ns();
	}
	
	return ns_formatted(duration);
}


void player_handle_tag_message(GstMessage *message)
{
	/*const GstStructure *s;
	
	s = gst_message_get_structure(message);
	g_print ("message from \"%s\" (%s): ",
	GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (message))),
				  gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
	if (s) {
		gchar *sstr;
		
		sstr = gst_structure_to_string (s);
		g_print ("%s\n", sstr);
		g_free (sstr);
	} else {
		g_print ("no message details\n");
	}*/
	
	GstTagList *tag_list;
	gchar *artist = NULL;
	gchar *title = NULL;
	
	gst_message_parse_tag(message, &tag_list);
	
	gst_tag_list_get_string(tag_list, GST_TAG_ARTIST, &artist);
	gst_tag_list_get_string(tag_list, GST_TAG_TITLE, &title);
	
	interface_set_songinfo(artist, title, 0);
	
	g_free(artist);
	g_free(title);
	gst_tag_list_free(tag_list);
	
}



// Song Slider Event Handler
gboolean on_hscale_song_value_changed(GtkRange *range, gpointer user_data)
{
	gdouble pos = 0;

	pos = gtk_range_get_value (range);

	g_print("Song Slider Value: %.0f\n", pos);

	return FALSE;
}


void on_trackplay_clicked(GtkButton *button, gpointer user_data)
{
	if (pipeline == NULL) {
		GtkWidget *msgbox;
		
		msgbox = gtk_message_dialog_new(GTK_WINDOW(mainwindow), GTK_DIALOG_MODAL,
										GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
										"Zuerst muss ein Stream erstellt werden!");
		gtk_window_set_title(GTK_WINDOW(msgbox), "Information");
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(msgbox);
		
		return;
	}
	
	if (interface_get_playing()) {
		g_print("Pause wurde gedrückt\n");
		player_set_stop();
	} else {
		g_print("Play wurde gedrückt\n");
		player_set_play();
	}
}

void on_trackstopp_clicked(GtkButton *button, gpointer user_data)
{
	if (pipeline == NULL) {
		GtkWidget *msgbox;
		
		msgbox = gtk_message_dialog_new(GTK_WINDOW(mainwindow), GTK_DIALOG_MODAL,
										GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
										"Zuerst muss ein Stream erstellt werden!");
		gtk_window_set_title(GTK_WINDOW(msgbox), "Information");
		gtk_dialog_run(GTK_DIALOG(msgbox));
		gtk_widget_destroy(msgbox);
		
		return;
	}
	
	g_print("Stop wurde gedrückt\n");
	player_set_stop();
}


gboolean on_progress_song_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Slider Button pressed!\n");

	interface_set_slidermove(TRUE);
	player_start_seek(widget, event, user_data);

	return FALSE;
}

// Handler für seeking
gboolean on_progress_song_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	g_print("Slider Button released!\n");

	GtkProgressBar *progress;
	
	progress = GTK_PROGRESS_BAR(widget);
	player_stop_seek(GTK_WIDGET(progress), user_data);
	interface_set_slidermove(FALSE);

	return FALSE;
}


