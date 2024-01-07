#include "startup.h"

void window_destroy(GtkWidget* window, gpointer data){
	printf("Bye bye\n");
	remove_all_controls(window);
	gtk_main_quit();
}

int main(int argc, char **argv){
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Bank App");
	gtk_window_set_default_size(GTK_WINDOW(window), 1300, 950);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	g_signal_connect(window, "destroy", G_CALLBACK(window_destroy), NULL);

	// run_startup(window);
	run_app(window, 1);
	
	/*struct Transaction *tr_list = get_account_transactions(3);
	for(int i = 0; tr_list[i].tr_id != -1; i++){
		char str[150];
		printf("%s\n", transaction_to_string(tr_list[i], str));
	}*/

	gtk_widget_show_all(window);
	gtk_main();
}
