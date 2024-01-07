#ifndef startup_h
#define startup_h

#include "app.h"

gpointer login_args[3];
gpointer signup_args[4];

void init_login_grid(GtkWidget *button, gpointer data);
void init_signup_grid(GtkWidget *button, gpointer data);

void add_spacing_label(GtkWidget *parent, int a, int b, int h){
	GtkWidget *empty_label = gtk_label_new("");
	gtk_widget_set_size_request(empty_label, 10, h);
	gtk_grid_attach(GTK_GRID(parent), empty_label, a, b, 1, 1);
}

GtkWidget* create_entry(char *text){
	GtkWidget *entry = gtk_entry_new();
	gtk_widget_set_size_request(entry, 150, 20);
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry), text);
	return entry;
}

void change_fore_color(GtkEntry *entry, int *rgb){
  // create attribute list
  PangoAttrList *attr_list = pango_attr_list_new();
  PangoAttribute *color_attr = pango_attr_foreground_new(rgb[0], rgb[1], rgb[2]);
  pango_attr_list_insert(attr_list, color_attr);

  // set the text attributes for the entry
  gtk_entry_set_attributes(entry, attr_list);

  // free the attribute list (it will be copied internally)
  pango_attr_list_unref(attr_list); 
}

void login_click(GtkWidget *button, gpointer data){
	GtkWidget *parent = GTK_WIDGET(data); // window
	
	const gchar *g_username = gtk_entry_get_text(GTK_ENTRY(login_args[1]));
	const gchar *g_password = gtk_entry_get_text(GTK_ENTRY(login_args[2]));
	
	char *username = g_strdup(g_username);
	char *password = g_strdup(g_password);
	
	struct User demo_user = get_by_username(username);
	int red[3] = {65535, 0, 0};
	int green [3] = {0, 65535, 0};
	
	if(demo_user.id == -1){
		if(strlen(username))
			change_fore_color(GTK_ENTRY(login_args[1]), red);
	}
	else if(!equal_strings(password, demo_user.password)){
		if(strlen(username))
			change_fore_color(GTK_ENTRY(login_args[1]), green);
		if(strlen(password))
			change_fore_color(GTK_ENTRY(login_args[2]), red);
	}
	else{
		run_app(parent, demo_user.id);
	}
}

void init_login_grid(GtkWidget *button, gpointer data){
	GtkWidget *parent = GTK_WIDGET(data); // window
	remove_all_controls(parent);
	
	GtkWidget* login_grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(parent), login_grid);
	gtk_widget_set_size_request(login_grid, 250, 250);
	gtk_widget_set_halign(login_grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(login_grid, GTK_ALIGN_CENTER);
	
	GtkWidget *entry_username = create_entry("Username");
	GtkWidget *entry_password = create_entry("Password");
	gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry_password), '*');
	
	GtkWidget *login_btn = gtk_button_new_with_label("Log me in");
	gtk_widget_set_size_request(login_btn, 100, 50);
	
	login_args[0] = data;
	login_args[1] = entry_username;
	login_args[2] = entry_password;
	g_signal_connect(login_btn, "clicked", G_CALLBACK(login_click), data);
	
	GtkWidget *signup_btn = gtk_button_new_with_label("No account? Create one here!");
	gtk_widget_set_size_request(signup_btn, 100, 50);
	g_signal_connect(signup_btn, "clicked", G_CALLBACK(init_signup_grid), data);
	
	gtk_grid_attach(GTK_GRID(login_grid), entry_username, 0, 0, 1, 1);
	add_spacing_label(login_grid, 0, 1, 7);
	gtk_grid_attach(GTK_GRID(login_grid), entry_password, 0, 2, 1, 1);
	add_spacing_label(login_grid, 0, 3, 30);
	gtk_grid_attach(GTK_GRID(login_grid), login_btn, 0, 4, 1, 1);
	add_spacing_label(login_grid, 0, 5, 15);
	gtk_grid_attach(GTK_GRID(login_grid), signup_btn, 0, 6, 1, 1);	
	
	gtk_widget_show_all(parent);
}

void signup_click(GtkWidget *button, gpointer data){
	GtkWidget *parent = GTK_WIDGET(data); // window
	
	char *username = g_strdup(gtk_entry_get_text(GTK_ENTRY(signup_args[1])));
	char *password = g_strdup(gtk_entry_get_text(GTK_ENTRY(signup_args[2])));
	char *password_confirm = g_strdup(gtk_entry_get_text(GTK_ENTRY(signup_args[3])));
	
	int red[3] = {65535, 0, 0};
	int green [3] = {0, 65535, 0};
	
	struct User ghost_user = get_by_username(username);
	if(ghost_user.id != -1 || !strlen(username)){
		if(strlen(username))
			change_fore_color(GTK_ENTRY(signup_args[1]), red);
	}
	else{
		change_fore_color(GTK_ENTRY(signup_args[1]), green);
		if(!equal_strings(password, password_confirm) || 
		   !strlen(password) || 
		   !strlen(password_confirm)){
			if(strlen(password_confirm))
				change_fore_color(GTK_ENTRY(signup_args[3]), red);
		}
		else{
			sign_up(username, password);
			run_app(parent, get_last_user_id());
		}
	}
	
}

void init_signup_grid(GtkWidget *button, gpointer data){
	GtkWidget *parent = GTK_WIDGET(data); // window
	remove_all_controls(parent);
	
	GtkWidget* signup_grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(parent), signup_grid);
	gtk_widget_set_size_request(signup_grid, 250, 350);
	gtk_widget_set_halign(signup_grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(signup_grid, GTK_ALIGN_CENTER);
	
	GtkWidget *entry_username = create_entry("Username");
	GtkWidget *entry_password = create_entry("Password");
	GtkWidget *entry_password_confirm = create_entry("Confirm password");
	gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry_password), '*');
    gtk_entry_set_visibility(GTK_ENTRY(entry_password_confirm), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry_password_confirm), '*');
	
	GtkWidget *signup_btn = gtk_button_new_with_label("Sign up!");
	gtk_widget_set_size_request(signup_btn, 100, 50);
	g_signal_connect(signup_btn, "clicked", G_CALLBACK(signup_click), data);
	
	signup_args[0] = data;
	signup_args[1] = entry_username;
	signup_args[2] = entry_password;
	signup_args[3] = entry_password_confirm;
	
	GtkWidget *login_btn = gtk_button_new_with_label("Already having an account?\nLogin here");
	gtk_widget_set_size_request(login_btn, 100, 50);
	g_signal_connect(login_btn, "clicked", G_CALLBACK(init_login_grid), data);
	
	gtk_grid_attach(GTK_GRID(signup_grid), entry_username, 0, 0, 1, 1);
	add_spacing_label(signup_grid, 0, 1, 7);
	gtk_grid_attach(GTK_GRID(signup_grid), entry_password, 0, 2, 1, 1);
	add_spacing_label(signup_grid, 0, 3, 7);
	gtk_grid_attach(GTK_GRID(signup_grid), entry_password_confirm, 0, 4, 1, 1);
	add_spacing_label(signup_grid, 0, 5, 30);
	gtk_grid_attach(GTK_GRID(signup_grid), signup_btn, 0, 6, 1, 1);
	add_spacing_label(signup_grid, 0, 7, 15);
	gtk_grid_attach(GTK_GRID(signup_grid), login_btn, 0, 8, 1, 1);	
	
	gtk_widget_show_all(parent);
}

void init_buttons_grid(GtkWidget *parent){
	GtkWidget *grid = gtk_grid_new();
	gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

	add_spacing_label(grid, 0, 0, 50);
	
	GtkWidget *login = gtk_button_new_with_label("Login");
	gtk_widget_set_size_request(login, 100, 50);
	gtk_grid_attach(GTK_GRID(grid), login, 0, 1, 1, 1);
	g_signal_connect(login, "clicked", G_CALLBACK(init_login_grid), parent);
	
	add_spacing_label(grid, 0, 2, 20);
	
	GtkWidget *signup = gtk_button_new_with_label("Sign up");
	gtk_widget_set_size_request(signup, 100, 50);
	gtk_grid_attach(GTK_GRID(grid), signup, 0, 3, 1, 1);
	g_signal_connect(signup, "clicked", G_CALLBACK(init_signup_grid), parent);
	
	gtk_container_add(GTK_CONTAINER(parent), grid);
}

void run_startup(GtkWidget *window){
	init_buttons_grid(window);
	// run_app(window, 1);
}

#endif
