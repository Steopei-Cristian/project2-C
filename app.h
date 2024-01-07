#ifndef app_h
#define app_h

#include "user.h"
#include "account.h"
#include "transaction.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <poppler/glib/poppler.h>

GtkWidget *back_btn;
struct User current_user;
int curr_acc_id;

int stack[100], top;

struct Actions_ClickData{
	GtkWidget *app_grid;
	int acc_id;
};
struct Deposit_ClickData{
	GtkWidget *entry_money;
	GtkWidget *entry_desc;
	int acc_id;
};
struct Transfer_ClickData{
	GtkWidget *to;
	GtkWidget *owner;
	GtkWidget *sum;
	GtkWidget *desc;
	int sender_id;
};
struct Report_ClickData{
	GtkWidget **scrolls;
	GtkWidget *from;
	GtkWidget *to;
	int acc_id;
};
struct Export_ClickData{
	GtkWidget *grid;
	char file[50];
	GtkWidget **scrolls;
};

int init_my_accounts(GtkWidget *app_grid);
void create_acc_menu(gpointer user_data);
void init_report(gpointer user_data);

void destroy_widget(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(widget);
}
void remove_all_controls(GtkWidget *parent){
    gtk_container_foreach(GTK_CONTAINER(parent), destroy_widget, NULL);
}

void back_click(GtkWidget *button, GtkWidget *app_grid){
	int current = stack[top];
	top--;
	
	if(current == 1){ //in account menu
		init_my_accounts(app_grid);
	}
	else if(current == 2){ //in one of account actions
		struct Actions_ClickData *data = g_new(struct Actions_ClickData , 1);
		data->app_grid = app_grid;
		data->acc_id = curr_acc_id;
		create_acc_menu(data);
	}
	else if(current == 3){
		struct Actions_ClickData *data = g_new(struct Actions_ClickData , 1);
		data->app_grid = app_grid;
		data->acc_id = curr_acc_id;
		init_report(data);
	}









}

void load_back(GtkWidget *parent, int a, int b){
	GtkWidget *back = gtk_button_new_with_label("Back");
	gtk_widget_set_name(back, "back-button");
	gtk_widget_set_size_request(back, 30, 15);
	gtk_widget_set_hexpand(back, FALSE);
	
	gtk_grid_attach(GTK_GRID(parent), back, a, b, 1, 1);
	g_signal_connect(back, "clicked", G_CALLBACK(back_click), parent);
	
	back_btn = back;
}

void apply_css(GtkWidget *app_grid){
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_file(provider, g_file_new_for_path("app_style.css"), NULL);
	GtkStyleContext *context = gtk_widget_get_style_context(app_grid);
	gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);
}


GtkWidget *create_tr_card(struct Transaction tr){
	char dollar_balance[30];
	get_dollar_balance(tr.balance, dollar_balance);
	
	char type[20] = "";
	if(tr.type == 1)
		strcpy(type, "Deposit");
	else if(tr.type == 2)
		strcpy(type, "Expense");
	else if(tr.type == 3)
		strcpy(type, "Withdrawal");
	else 
		strcpy(type, "Income");


	GtkWidget *card = gtk_grid_new();
	gtk_widget_set_name(card, "tr-card");
	gtk_widget_set_size_request(card, 200, 25);
	gtk_widget_set_hexpand(card, FALSE);
	
	char cols [5][100];
	strcpy(cols[0], "Transaction ID\n\n");
	strcpy(cols[1], "Type\n\n");
	strcpy(cols[2], "Sum\n\n");
	strcpy(cols[3], "Description\n\n");
	strcpy(cols[4], "Date\n\n");
	
	//define labels
	char id_str[10];
	itoa(tr.tr_id, id_str);
	strcat(cols[0], id_str);
	strcat(cols[1], type);
	strcat(cols[2], dollar_balance);
	strcat(cols[3], tr.description);
	strcat(cols[4], tr.created_at);
	
	for(int i = 0; i < 5; i++){
		GtkWidget *label = gtk_label_new(cols[i]);
		gtk_label_set_xalign(GTK_LABEL(label), 0.5);
		if(i != 4)
			gtk_widget_set_name(label, "tr-card-label");
		else 
			gtk_widget_set_name(label, "last-tr-card-label");
		gtk_widget_set_size_request(label, 40, 25);
		gtk_grid_attach(GTK_GRID(card), label, i, 0, 1, 1);
	}

	return card;
}

void display_transactions(struct Transaction *tr, GtkWidget *app_grid){
	remove_all_controls(app_grid);
	
	load_back(app_grid, 0, 0);
	
	GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
								   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scroll, 300, 300);
	gtk_widget_set_margin_top(scroll, 50);
	
	GtkWidget *cards_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_name(cards_box, "tr-card-box");
	
	for(int i = 0; tr[i].tr_id != -1; i++){
		GtkWidget *card = create_tr_card(tr[i]);
		gtk_box_pack_start(GTK_BOX(cards_box), card, FALSE, FALSE, 0);
	}
	
	gtk_container_add(GTK_CONTAINER(scroll), cards_box);
	gtk_grid_attach(GTK_GRID(app_grid), scroll, 0, 2, 1, 1);
	
	apply_css(app_grid);
	
	stack[++top] = 2;
	
	gtk_widget_show_all(app_grid);
}

void view_tr_click(GtkWidget *button, gpointer user_data){
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;

	struct Transaction tr_list[1000];
	get_account_transactions(data->acc_id, tr_list);
	display_transactions(tr_list, data->app_grid);	
}



void deposit_click(GtkWidget *button, gpointer user_data){
	struct Deposit_ClickData *data = (struct Deposit_ClickData *)user_data;
	
	//check float for MONEY
	float money = atof(g_strdup(gtk_entry_get_text(GTK_ENTRY(data->entry_money))));
	char *desc = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->entry_desc)));
	
	GDateTime *current_time = g_date_time_new_now_local();
    const gchar *formatted_date = g_date_time_format(current_time, "%d.%m.%Y");
    char *date = g_strdup(formatted_date);

	struct Transaction new_tr = create_transaction(get_last_tr_id() + 1, data->acc_id, 1,
												   money, desc, date, 0, 0);
	add_transaction(new_tr);
	struct Account account = get_account_by_id(data->acc_id);
	add_balance(&account, money);
	char acc_str[100];
	account_to_string(account, acc_str);
	replace_account_line(account.account_id, acc_str); 
}

void init_deposit(GtkWidget *button, gpointer user_data){
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;
	
	remove_all_controls(data->app_grid);

	load_back(data->app_grid, 0, 0);
	gtk_widget_set_margin_start(back_btn, 30);
	struct Account account = get_account_by_id(data->acc_id);
	
	char message_str[100] = "Deposit money in ";
	strcat(message_str, account.account_name);
	GtkWidget *message = gtk_label_new(message_str);
	gtk_widget_set_name(message, "deposit-message");
	gtk_widget_set_margin_bottom(message, 20);
	gtk_widget_set_margin_top(message, 50);
	gtk_grid_attach(GTK_GRID(data->app_grid), message, 0, 1, 1, 1);
	
	GtkWidget *entry = gtk_entry_new();
	gtk_widget_set_name(entry, "deposit-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Sum ($)");
	gtk_widget_set_margin_bottom(entry, 20);
	gtk_grid_attach(GTK_GRID(data->app_grid), entry, 0, 2, 1, 1);
	
	GtkWidget *desc = gtk_entry_new();
	gtk_widget_set_name(desc, "deposit-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(desc), "Description");
	gtk_widget_set_margin_bottom(desc, 20);
	gtk_grid_attach(GTK_GRID(data->app_grid), desc, 0, 3, 1, 1);
	
	GtkWidget *deposit_btn = gtk_button_new_with_label("Deposit");
	gtk_widget_set_size_request(deposit_btn, 50, -1);
	gtk_widget_set_name(deposit_btn, "deposit-button");
	
	struct Deposit_ClickData *deposit_data = g_new(struct Deposit_ClickData, 1);
	deposit_data->entry_money = entry;
	deposit_data->entry_desc = desc;
	deposit_data->acc_id = data->acc_id;
	
	g_signal_connect(deposit_btn, "clicked", G_CALLBACK(deposit_click), deposit_data);
	gtk_grid_attach(GTK_GRID(data->app_grid), deposit_btn, 0, 4, 1, 1);
	
	apply_css(data->app_grid);
	
	stack[++top] = 2;
	
	gtk_widget_show_all(data->app_grid);
}

void transfer_click(GtkWidget *button, gpointer user_data){
	struct Transfer_ClickData *data = (struct Transfer_ClickData *)user_data;
	
	// check integer for TO and float for SUM
	int to_id = atoi(g_strdup(gtk_entry_get_text(GTK_ENTRY(data->to))));
	char *owner_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->owner)));
	float sum = atof(g_strdup(gtk_entry_get_text(GTK_ENTRY(data->sum))));
	char *desc = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->desc)));
	GDateTime *current_time = g_date_time_new_now_local();
	char *time = g_strdup(g_date_time_format(current_time, "%d.%m.%Y"));
	
	struct Account sender = get_account_by_id(data->sender_id);
	struct Account receiver = get_account_by_id(to_id);
	struct User receiver_owner = get_by_username(owner_name);
	
	if(receiver.account_id == -1){
		printf("%s\n", "Receiver not found");
	}
	else if(receiver_owner.id == -1){
		g_print("%s\n", "Owner name not valid");
	}
	else if(sum > sender.balance){
		printf("%s\n", "Can t afford transfer");
	}
	else{
		struct Transaction expense = create_transaction(get_last_tr_id() + 1, sender.account_id, 
										2, sum, desc, time, sender.account_id, receiver.account_id);
		add_transaction(expense);
		struct Transaction income = create_transaction(get_last_tr_id() + 1, receiver.account_id, 
										4, sum, desc, time, sender.account_id, receiver.account_id);
		add_transaction(income);
		
		sub_balance(&sender, sum);
		add_balance(&receiver, sum);
		
		char acc_str[100];
		account_to_string(sender, acc_str);
		replace_account_line(sender.account_id, acc_str);
		account_to_string(receiver, acc_str);
		replace_account_line(receiver.account_id, acc_str); 
	}
	
}

void init_transfer(GtkWidget *button, gpointer user_data){
	struct Actions_ClickData  *data = (struct Actions_ClickData *)user_data;
	
	remove_all_controls(data->app_grid);
	
	load_back(data->app_grid, 1, 0);
	
	struct Account account = get_account_by_id(data->acc_id);
	
	GtkWidget *to = gtk_entry_new();
	gtk_widget_set_name(to, "transfer-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(to), "Account code of receiver");
	gtk_widget_set_margin_top(to, 15);
	gtk_widget_set_margin_bottom(to, 10);
	gtk_widget_set_margin_end(to, 10);
	gtk_widget_set_size_request(to, 250, -1);
	gtk_grid_attach(GTK_GRID(data->app_grid), to, 0, 1, 1, 1);
	
	GtkWidget *ghost_label1 = gtk_label_new("");
	gtk_widget_set_size_request(ghost_label1, 5, 2);
	gtk_grid_attach(GTK_GRID(data->app_grid), ghost_label1, 1, 1, 1, 1);
	
	GtkWidget *owner = gtk_entry_new();
	gtk_widget_set_name(owner, "transfer-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(owner), "Name of the receiver");
	gtk_widget_set_margin_top(owner, 15);
	gtk_widget_set_margin_bottom(owner, 10);
	gtk_widget_set_size_request(owner, 250, -1);
	gtk_grid_attach(GTK_GRID(data->app_grid), owner, 2, 1, 1, 1);
	
	GtkWidget *entry = gtk_entry_new();
	gtk_widget_set_name(entry, "transfer-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Sum ($)");
	gtk_widget_set_margin_bottom(entry, 20);
	gtk_widget_set_margin_end(entry, 10);
	gtk_widget_set_size_request(entry, 100, 20);
	gtk_grid_attach(GTK_GRID(data->app_grid), entry, 0, 2, 1, 1);
	
	GtkWidget *desc = gtk_entry_new();
	gtk_widget_set_name(desc, "transfer-entry");
	gtk_entry_set_placeholder_text(GTK_ENTRY(desc), "Description");
	gtk_widget_set_size_request(desc, 100, -1);
	gtk_widget_set_margin_bottom(desc, 20);
	gtk_grid_attach(GTK_GRID(data->app_grid), desc, 2, 2, 1, 1);
	
	GtkWidget *send = gtk_button_new_with_label("Transfer");
	gtk_widget_set_name(send, "transfer-button");
	
	struct Transfer_ClickData *transfer_data = g_new(struct Transfer_ClickData, 1);
	transfer_data->to = to;
	transfer_data->owner = owner;
	transfer_data->sum = entry;
	transfer_data->desc = desc;
	transfer_data->sender_id = data->acc_id;
	g_signal_connect(send, "clicked", G_CALLBACK(transfer_click), transfer_data);
	
	gtk_grid_attach(GTK_GRID(data->app_grid), send, 1, 3, 1, 1);
	gtk_widget_set_halign(send, GTK_ALIGN_CENTER);
	
	apply_css(data->app_grid);
	
	stack[++top] = 2;
	
	gtk_widget_show_all(data->app_grid);
}


void draw_grid_content(GtkWidget *grid, cairo_t *cr, gpointer data){
 	gtk_widget_queue_draw(grid);
 	gtk_widget_draw(grid, cr);
}

void export_grid_to_pdf(GtkWidget *button, gpointer user_data){
	struct Export_ClickData *data = (struct Export_ClickData *)user_data;
	
	cairo_surface_t *surface;
	cairo_t *cr;
	
	surface = cairo_pdf_surface_create(data->file, 
									   gtk_widget_get_allocated_width(data->grid),
									   gtk_widget_get_allocated_height(data->grid));
									   
	cr = cairo_create(surface);
	draw_grid_content(data->grid, cr, NULL);
	
	cairo_surface_finish(surface);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}





































void make_statement(GtkWidget *button, gpointer user_data){
	struct Report_ClickData *data = (struct Report_ClickData *)user_data;
	
	char *start = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->from)));
	char *end = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->to)));
	
	char types[4][20];
	strcpy(types[0], "Deposits");
	strcpy(types[1], "Expenses");
	strcpy(types[2], "Withdrawals");
	strcpy(types[3], "Incomes");
	
	for(int i = 0; i < 4; i++){
		struct Transaction tr[100];
		get_transactions_by_type_in_interval(data->acc_id, i + 1, start, end, tr);
		
		GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(data->scrolls[i]));
		GtkWidget *cards_box = gtk_bin_get_child(GTK_BIN(viewport));
		remove_all_controls(cards_box);
		
		GtkWidget *title = gtk_label_new(types[i]);
		gtk_widget_set_name(title, "statement-box-title");
		gtk_box_pack_start(GTK_BOX(cards_box), title, FALSE, FALSE, 0);
		
		for(int j = 0; tr[j].tr_id != -1; j++){
			GtkWidget *card = create_tr_card(tr[j]);
			gtk_box_pack_start(GTK_BOX(cards_box), card, FALSE, FALSE, 0);
		}
		
		gtk_widget_show_all(data->scrolls[i]);
	}
}

GtkWidget *make_statement_scrolledw_category(char *category){
	GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
								   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scroll, 200, 200);
	
	GtkWidget *cards_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_name(cards_box, "statement-card-box");
	
	GtkWidget *title = gtk_label_new(category);
	gtk_widget_set_name(title, "statement-box-title");
	gtk_box_pack_start(GTK_BOX(cards_box), title, FALSE, FALSE, 0);
	
	gtk_container_add(GTK_CONTAINER(scroll), cards_box);
	
	return scroll;
}

void generate_click(GtkWidget *button, gpointer user_data){
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;
	struct Account account = get_account_by_id(data->acc_id);
	
	remove_all_controls(data->app_grid);
	
	load_back(data->app_grid, 1, 0);
	
	char mes[100] = "Account statement:\n";
	strcat(mes, account.account_name);
	GtkWidget *message = gtk_label_new(mes);
	gtk_widget_set_name(message, "statement-message");
	gtk_widget_set_margin_top(message, 15);
	gtk_grid_attach(GTK_GRID(data->app_grid), message, 1, 1, 1, 1);
	
	GtkWidget *from = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(from), "Starting from:");
	gtk_widget_set_name(from, "statement-entry");
	gtk_widget_set_margin_top(from, 15);
	gtk_widget_set_margin_bottom(from, 5);
	gtk_grid_attach(GTK_GRID(data->app_grid), from, 1, 2, 1, 1);
	
	GtkWidget *to = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(to), "Up to:");
	gtk_widget_set_name(to, "statement-entry");
	gtk_widget_set_margin_bottom(to, 25);
	gtk_grid_attach(GTK_GRID(data->app_grid), to, 1, 3, 1, 1);
	
	char types[4][20];
	strcpy(types[0], "Deposits");
	strcpy(types[1], "Expenses");
	strcpy(types[2], "Withdrawals");
	strcpy(types[3], "Incomes");
	
	GtkWidget **categories = g_new(GtkWidget *, 4);
	for(int i = 0; i < 4; i++){
		categories[i] = make_statement_scrolledw_category(types[i]);
		gtk_widget_set_name(categories[i], "report-category-box");
		int a = (i % 2) ? 2 : 0;
		int b = (i < 2) ? 4 : 5;
		gtk_grid_attach(GTK_GRID(data->app_grid), categories[i], a, b, 1, 1);
		
	}
	
	struct Report_ClickData *report_data = g_new(struct Report_ClickData, 1);
	report_data->scrolls = categories;
	report_data->from = from;
	report_data->to = to;
	report_data->acc_id = data->acc_id;
	
	GtkWidget *generate = gtk_button_new_with_label("Generate");
	gtk_widget_set_name(generate, "gen-report-button");
	gtk_grid_attach(GTK_GRID(data->app_grid), generate, 1, 6, 1, 1);
	g_signal_connect(generate, "clicked", G_CALLBACK(make_statement), report_data);
	
	struct Export_ClickData *export_data = g_new(struct Export_ClickData, 1);
	export_data->grid = data->app_grid;
	strcpy(export_data->file, "statement.pdf");
	export_data->scrolls = categories;
	
	GtkWidget *export_pdf = gtk_button_new_with_label("Export to statement.pdf");
	gtk_widget_set_name(export_pdf, "exp-pdf-button");
	g_signal_connect(export_pdf, "clicked", G_CALLBACK(export_grid_to_pdf), export_data);
	gtk_grid_attach(GTK_GRID(data->app_grid), export_pdf, 1, 7, 1, 1); 
	
	stack[++top] = 3;	
	
	gtk_widget_show_all(data->app_grid);
}

void init_report(gpointer user_data){
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;
	
	remove_all_controls(data->app_grid);
	
	load_back(data->app_grid, 0, 0);
	gtk_widget_set_margin_start(back_btn, 50);
	
	GtkWidget *generate_btn = gtk_button_new_with_label("Generate account statement");
	gtk_widget_set_name(generate_btn, "report-button");
	gtk_widget_set_margin_start(generate_btn, 50);
	gtk_widget_set_margin_top(generate_btn, 30);
	gtk_widget_set_size_request(generate_btn, 50, 25);
	g_signal_connect(generate_btn, "clicked", G_CALLBACK(generate_click), user_data);
	gtk_grid_attach(GTK_GRID(data->app_grid), generate_btn, 0, 1, 1, 1);
	
	GtkWidget *import_btn = gtk_button_new_with_label("Import account statement");
	gtk_widget_set_name(import_btn, "report-button");
	gtk_widget_set_margin_start(import_btn, 50);
	gtk_widget_set_size_request(import_btn, 50, 25);
	gtk_grid_attach(GTK_GRID(data->app_grid), import_btn, 0, 2, 1, 1);
	
	apply_css(data->app_grid);
	
	stack[++top] = 2;
	
	gtk_widget_show_all(data->app_grid);
}

void report_clicked(GtkWidget *button, gpointer user_data){
	init_report(user_data);
}

void create_acc_menu(gpointer user_data){
	int next = 0;
	
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;
	GtkWidget *app_grid = data->app_grid;
	int acc_id = data->acc_id;
	curr_acc_id = acc_id;
	struct Account account = get_account_by_id(acc_id);
	
	remove_all_controls(app_grid);
	
	GtkWidget *btn_view = gtk_button_new_with_label("View transactions");
	gtk_widget_set_name(btn_view, "acc-menu-button");
	gtk_widget_set_size_request(btn_view, 100, 50);
	gtk_grid_attach(GTK_GRID(app_grid), btn_view, 0, 0, 1, 1);
	g_signal_connect(btn_view, "clicked", G_CALLBACK(view_tr_click), user_data); 
	next++;
	
	GtkWidget *btn_deposit = gtk_button_new_with_label("Deposit money");
	gtk_widget_set_name(btn_deposit, "acc-menu-button");
	gtk_widget_set_size_request(btn_deposit, 100, 50);
	gtk_grid_attach(GTK_GRID(app_grid), btn_deposit, 1, 0, 1, 1);
	g_signal_connect(btn_deposit, "clicked", G_CALLBACK(init_deposit), user_data);
	next++;
	
	if(account.type == 1){
		GtkWidget *btn_send = gtk_button_new_with_label("Send money");
		gtk_widget_set_name(btn_send, "acc-menu-button");
		gtk_widget_set_size_request(btn_send, 100, 50);
		g_signal_connect(btn_send, "clicked", G_CALLBACK(init_transfer), user_data);
		gtk_grid_attach(GTK_GRID(app_grid), btn_send, 0, 1, 1, 1);
		next++;
	}
	
	GtkWidget *btn_report = gtk_button_new_with_label("Report menu");
	gtk_widget_set_name(btn_report, "acc-menu-button");
	gtk_widget_set_size_request(btn_report, 100, 50);
	g_signal_connect(btn_report, "clicked", G_CALLBACK(report_clicked), user_data);
	
	if(next != 3)
		gtk_grid_attach(GTK_GRID(app_grid), btn_report, 0, 1, 1, 1);
	else 
		gtk_grid_attach(GTK_GRID(app_grid), btn_report, 1, 1, 1, 1);
	next++;
	
	/*GtkWidget *btn_export = gtk_button_new_with_label("Export transactions\n(Excel or pdf)");
	gtk_widget_set_name(btn_export, "acc-menu-button");
	gtk_widget_set_size_request(btn_export, 100, 50);
	if(next != 4)
		gtk_grid_attach(GTK_GRID(app_grid), btn_export, 1, 1, 1, 1);
	else 
		gtk_grid_attach(GTK_GRID(app_grid), btn_export, 0, 2, 1, 1);*/
	
	load_back(app_grid, 2, 0);
	apply_css(app_grid);
	
	stack[++top] = 1;
	
	gtk_widget_show_all(app_grid);
}

void actions_click(GtkWidget *button, gpointer user_data){
	struct Actions_ClickData *data = (struct Actions_ClickData *)user_data;
	create_acc_menu(user_data);
}

GtkWidget *create_account_treeview(struct Account account){
	char cols [4][100];
	strcpy(cols[0], "Account ID");
	strcpy(cols[1], "Owner");
	strcpy(cols[2], "Type");
	strcpy(cols[3], "Balance");
	
	enum{
		ACC_ID_COL,
		OWNER_COL,
		TYPE_COL,
		BALANCE_COL,
		NUM_COLS
	};
	
	GtkListStore *list;
	list = gtk_list_store_new(NUM_COLS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	char dollar_balance[10];
	get_dollar_balance(account.balance, dollar_balance);
	
	GtkTreeIter iter;
	gtk_list_store_append(list, &iter);
	gtk_list_store_set(list, &iter, 
					   ACC_ID_COL, account.account_id,
					   OWNER_COL, current_user.username, 
					   TYPE_COL, (account.type == 1 ? "Checking" : "Savings"),
					   BALANCE_COL, dollar_balance,
					   -1);
					   
	GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
	gtk_widget_set_size_request(treeview, -1, 30);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	for(int i = 0; i < NUM_COLS; i++){
		renderer = gtk_cell_renderer_text_new();
		gtk_cell_renderer_set_alignment(GTK_CELL_RENDERER(renderer), 0.5, 0.5);
		column = gtk_tree_view_column_new_with_attributes(cols[i], renderer, "text", i, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	}
	
	gtk_widget_set_sensitive(treeview, FALSE);
	gtk_widget_set_margin_end(treeview, 7);
	
	return treeview;
}

void create_account_grid(struct Account account, GtkWidget *app_grid, int i){
	GtkWidget *grid = gtk_grid_new();
	gtk_widget_set_name(grid, "account-grid");
	gtk_widget_set_size_request(grid, 500, 100);
	gtk_grid_attach(GTK_GRID(app_grid), grid, 0, i, 1, 1);
	
	gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 15);
	
	GtkWidget *treeview = create_account_treeview(account);
	gtk_widget_set_name(treeview, "account-tree-view");
	gtk_widget_set_margin_top(treeview, 5);
	gtk_widget_set_margin_start(treeview, 5);
	gtk_grid_attach(GTK_GRID(grid), treeview, 0, 0, 1, 1);
	
	GtkWidget *actions = gtk_button_new_with_label("Actions");
	gtk_widget_set_name(actions, "account-grid-button");
	gtk_grid_attach(GTK_GRID(grid), actions, 1, 0, 1, 1);
	gtk_widget_set_margin_end(actions, 5);
	
	struct Actions_ClickData *data = g_new(struct Actions_ClickData, 1);
	data->app_grid = app_grid;
	data->acc_id = account.account_id;
	g_signal_connect(actions, "clicked", G_CALLBACK(actions_click), data);
	
	GtkWidget *close_acc = gtk_button_new_with_label("Close account");
	gtk_widget_set_name(close_acc, "account-grid-button");
	gtk_widget_set_margin_end(close_acc, 5);
	gtk_grid_attach(GTK_GRID(grid), close_acc, 2, 0, 1, 1);
}

int init_my_accounts(GtkWidget *app_grid){
	remove_all_controls(app_grid);

	int i = 0;
	struct Account acc[5];
	get_owner_accounts(current_user.id, acc);
	for(i = 0; acc[i].account_id != -1; i++){
		create_account_grid(acc[i], app_grid, i);
	}

	gtk_widget_show_all(app_grid);
	return i;
}

void run_app(GtkWidget *app_window, int user_id){
	current_user = get_user_by_id(user_id);
	
	remove_all_controls(app_window);
	gtk_widget_set_name(app_window, "app-window");	
	
	GtkWidget *app_grid = gtk_grid_new();
	gtk_widget_set_name(app_grid, "app-grid");
	gtk_widget_set_size_request(app_grid, 500, 500);
	gtk_widget_set_halign(app_grid, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(app_grid, GTK_ALIGN_CENTER);
	
	int rows = init_my_accounts(app_grid);
	apply_css(app_grid);
	
	gtk_container_add(GTK_CONTAINER(app_window), app_grid);
	gtk_widget_show_all(app_window);
}

#endif
