#ifndef transaction_h
#define transaction_h

#include "utils.h"

struct Transaction{
	int tr_id;
	int acc_id;
	int type; //1-deposit 2-expense 3-withdrawal 4-income
	float balance;
	char description[100];
	char created_at[25]; //dd.mm.yyy
	
	int from; // acc_id of the account who credited you
	int to; // acc_id of the account that you want to send money to
};

struct Transaction create_transaction(int tr_id, int acc_id, int type, 
									  float balance, char *desc, char *date,
									  int from, int to){
	struct Transaction res;
	
	res.tr_id = tr_id;
	res.acc_id = acc_id;
	res.type = type;
	res.balance = balance;
	strcpy(res.description, desc);
	strcpy(res.created_at, date);
	res.from = from;
	res.to = to;
	
	return res;
}

void transaction_to_string(struct Transaction tr, char *res){
	char id_str[10], acc_str[10], ty_str[10], bal_str[10], from_str[10], to_str[10];
	
	strcpy(res, "");
	strcat(res, itoa(tr.tr_id, id_str));
	strcat(res, "|");
	strcat(res, itoa(tr.acc_id, acc_str));
	strcat(res, "|");
	strcat(res, itoa(tr.type, ty_str));
	strcat(res, "|");
	strcat(res, ftoa(tr.balance, bal_str));
	strcat(res, "|");
	strcat(res, tr.description);
	strcat(res, "|");
	strcat(res, tr.created_at);
	strcat(res, "|");
	strcat(res, itoa(tr.from, from_str));
	strcat(res, "|");
	strcat(res, itoa(tr.to, to_str));
	strcat(res, "|");
}

struct Transaction get_transaction_from_line(char *line){
	char **fields = split_by_sep(line, '|', 8);
	
	struct Transaction tr = create_transaction(atoi(fields[0]), atoi(fields[1]), atoi(fields[2]), 
							  atof(fields[3]), fields[4], fields[5],
							  atoi(fields[6]), atoi(fields[7]));
	free(fields);
	return tr;
}

void get_account_transactions(int acc_id, struct Transaction* res){
	int k = 0;
	
	FILE *fin;
	fin = fopen("/home/cristi/project2/transactions.txt", "r");
	char tr_line[250];
	struct Transaction ghost_tr;
	
	while(fgets(tr_line, 250, fin)){
		ghost_tr = get_transaction_from_line(tr_line);
		if(ghost_tr.acc_id == acc_id){
			*(res + k) = ghost_tr;
			k++;
		}
	}
	
	fclose(fin);
	
	ghost_tr = create_transaction(-1, -1, -1, 0, "", "", 0, 0);
	*(res + k) = ghost_tr;
}

void get_transactions_by_type_in_interval(int acc_id, int ty, 
										  char *start, char *end, struct Transaction *res){	
	int k = 0;
	
	FILE *fin;
	fin = fopen("/home/cristi/project2/transactions.txt", "r");
	char tr_line[250];
	struct Transaction ghost_tr;
	
	while(fgets(tr_line, 250, fin)){
		ghost_tr = get_transaction_from_line(tr_line);
		if(ghost_tr.acc_id == acc_id && ghost_tr.type == ty && 
		   is_in_interval(ghost_tr.created_at, start, end)){
			*(res + k) = ghost_tr;
			k++;
		}
	}
	
	fclose(fin);
	
	ghost_tr = create_transaction(-1, -1, -1, 0, "", "", 0, 0);
	*(res + k) = ghost_tr;
}


int get_last_tr_id(){
	FILE *fin;
	fin = fopen("/home/cristi/project2/transactions.txt", "r");
	char tr_line[250];
	struct Transaction ghost_tr;
	
	while(fgets(tr_line, 250, fin)){
		ghost_tr = get_transaction_from_line(tr_line);
	}
	
	fclose(fin);
	
	return ghost_tr.tr_id;
}

void add_transaction(struct Transaction tr){
	char tr_line[250];
	transaction_to_string(tr, tr_line);
	
	FILE *fin;
	fin = fopen("/home/cristi/project2/transactions.txt", "a");
	fprintf(fin, "%s\n", tr_line);
	fclose(fin);
}


#endif


















