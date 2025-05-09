
#include "melodia_launcher-application.h"
#include "window/melodia_launcher-window.h"

struct _MelodiaLauncherApplication
{
  GtkApplication parent_instance;
};
//macro function, generates boilerplate code, params must follow naming convention
//after running this function, you now have access to the init and class_init functions
// eg. melodia_launcher_application_init, melodia_launcher_application_class_init
G_DEFINE_TYPE (MelodiaLauncherApplication, melodia_launcher_application, GTK_TYPE_APPLICATION)

MelodiaLauncherApplication *
melodia_launcher_application_new (gchar *application_id,
                                  GApplicationFlags  flags)
{
  return g_object_new (MELODIA_LAUNCHER_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       NULL);
}

static void
melodia_launcher_application_finalize (GObject *object)
{
  MelodiaLauncherApplication *self = (MelodiaLauncherApplication *)object;

  G_OBJECT_CLASS (melodia_launcher_application_parent_class)->finalize (object);
}

static void
melodia_launcher_application_activate (GApplication *app)
{
  GtkWindow *window;

  /* It's good practice to check your parameters at the beginning of the
   * function. It helps catch errors early and in development instead of
   * by your users.
   */
  g_assert (GTK_IS_APPLICATION (app));

  /* Get the current window or create one if necessary. */
  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window == NULL)
    window = g_object_new (MELODIA_LAUNCHER_TYPE_WINDOW,
                           "application", app,
                           NULL);

  /* Ask the window manager/compositor to present the window. */
  gtk_window_present (window);
}


static void
melodia_launcher_application_class_init (MelodiaLauncherApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  object_class->finalize = melodia_launcher_application_finalize;

  /*
   * We connect to the activate callback to create a window when the application
   * has been launched. Additionally, this callback notifies us when the user
   * tries to launch a "second instance" of the application. When they try
   * to do that, we'll just present any existing window.
   */
  app_class->activate = melodia_launcher_application_activate;
}

static void
melodia_launcher_application_show_about (GSimpleAction *action,
                                         GVariant      *parameter,
                                         gpointer       user_data)
{
  MelodiaLauncherApplication *self = MELODIA_LAUNCHER_APPLICATION (user_data);
  GtkWindow *window = NULL;
  const gchar *authors[] = {"Ryan", NULL};

  g_return_if_fail (MELODIA_LAUNCHER_IS_APPLICATION (self));

  window = gtk_application_get_active_window (GTK_APPLICATION (self));

  gtk_show_about_dialog (window,
                         "program-name", "melodia-launcher",
                         "authors", authors,
                         "version", "0.1.0",
                         NULL);
}


static void
melodia_launcher_application_init (MelodiaLauncherApplication *self)
{
  g_autoptr (GSimpleAction) quit_action = g_simple_action_new ("quit", NULL);
  g_signal_connect_swapped (quit_action, "activate", G_CALLBACK (g_application_quit), self);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (quit_action));

  g_autoptr (GSimpleAction) about_action = g_simple_action_new ("about", NULL);
  g_signal_connect (about_action, "activate", G_CALLBACK (melodia_launcher_application_show_about), self);
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (about_action));

  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.quit",
                                         (const char *[]) {
                                           "<primary>q",
                                           NULL,
                                         });
}
