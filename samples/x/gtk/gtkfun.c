/*
 * gtkfun.c
 * Vino Crescini
 * 18 june 2001
 */

#include <gtk/gtk.h>

void exitProgram(GtkButton *, gpointer);

int main(int argc, char *argv[])
{
  GtkWidget *topWidget;
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *separator;
  GtkWidget *exitButton;

  gtk_init(&argc, &argv);

  topWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  box = gtk_vbox_new(FALSE, 10);
  label = gtk_label_new("Hello World\n");
  separator = gtk_hseparator_new();
  exitButton = gtk_button_new_with_label("Exit");

  gtk_window_set_title(GTK_WINDOW(topWidget), "gtkfun");
  gtk_container_set_border_width(GTK_CONTAINER(topWidget), 15);
  gtk_widget_set_name(topWidget, "gtkfun");
  gtk_widget_set_name(box, "vertical box");
  gtk_widget_set_name(label, "label");
  gtk_widget_set_name(separator, "separator");
  gtk_widget_set_name(exitButton, "exit");

  gtk_signal_connect(GTK_OBJECT(exitButton), "clicked", exitProgram, NULL);
  
  gtk_container_add(GTK_CONTAINER(topWidget), box);
  gtk_box_pack_start_defaults(GTK_BOX(box), label);
  gtk_box_pack_start_defaults(GTK_BOX(box), separator);
  gtk_box_pack_start_defaults(GTK_BOX(box), exitButton);

  gtk_widget_show_all(topWidget);

  gtk_main();

  g_print("done\n");

  return 0;
}

void exitProgram(GtkButton *clicked, gpointer userData)
{
  g_print("called exitProgram();\n");
  gtk_main_quit();
}
