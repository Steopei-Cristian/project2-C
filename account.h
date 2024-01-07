#ifndef account_h
#define account_h

#include "utils.h"

struct Account{
	int account_id;
	int owner_id;
	int type; // 1-checking 2-savings 3-mai vedem
	float balance;
	char account_name[50];
};

void add_balance(struct Account *acc, float money){
	acc->balance += money;
}
void sub_balance(struct Account *acc, float money){
	acc->balance -= money;
}

struct Account create_account(int input_acc_id, int input_owner_id, 
							  int input_type, float input_balance, char *input_acc_name){
	struct Account new_acc;
	new_acc.account_id = input_acc_id;
	new_acc.owner_id = input_owner_id;
	new_acc.type = input_type;
	new_acc.balance = input_balance;
	strcpy(new_acc.account_name, input_acc_name);
	return new_acc;							  
}

void account_to_string(struct Account account, char* res){
	char acc_id[10], owner_id[10], ty[10], bal[10];
	
	strcpy(res, "");
	strcat(res, itoa(account.account_id, acc_id));
	strcat(res, "|");
	strcat(res, itoa(account.owner_id, owner_id));
	strcat(res, "|");
	strcat(res, itoa(account.type, ty));
	strcat(res, "|");
	strcat(res, ftoa(account.balance, bal));
	strcat(res, "|");
	strcat(res, account.account_name);
	strcat(res, "|");
}

struct Account get_account_from_line(char *line){
	char **fields = split_by_sep(line, '|', 5);
	struct Account acc = create_account(atoi(fields[0]), atoi(fields[1]), 
						  atoi(fields[2]), atof(fields[3]), fields[4]);
	free(fields);
	return acc;
}

void get_owner_accounts(int input_owner_id, struct Account *res){
	int k = 0;
	
	FILE *fin;
	fin = fopen("/home/cristi/project2/accounts.txt", "r");
	char account_line[100];
	struct Account ghost_account;
	
	while(fgets(account_line, 100, fin)){
		ghost_account = get_account_from_line(account_line);
		if(ghost_account.owner_id == input_owner_id){
			*(res + k) = ghost_account;
			k++;
		}
	}
	
	fclose(fin);
	
	ghost_account = create_account(-1, -1, 0, 0, "");
	*(res + k) = ghost_account;
}

struct Account get_account_by_id(int input_account_id){
	FILE *fin;
	fin = fopen("/home/cristi/project2/accounts.txt", "r");
	char account_line[100];
	struct Account ghost_account;
	
	while(fgets(account_line, 100, fin)){
		ghost_account = get_account_from_line(account_line);
		if(ghost_account.account_id == input_account_id){
			fclose(fin);
			return ghost_account;
		}
	}
	
	fclose(fin);
	
	ghost_account = create_account(-1, -1, 0, 0, "");
	return ghost_account;
}

void replace_account_line(int acc_id, char *new_line){
	FILE *fin = fopen("/home/cristi/project2/accounts.txt", "r");
	FILE *temp = tmpfile();
	
	char acc_line[100], temp_str[100];
	struct Account ghost_account;
	int ok = 1;
	
	while(ok && fgets(acc_line, 100, fin) != NULL){
		ghost_account = get_account_from_line(acc_line);
		/*account_to_string(ghost_account, temp_str);*/
		if(ghost_account.account_id != acc_id){
			strcat(acc_line, "");
			fputs(acc_line, temp);
		}
			//fprintf(temp, "%s\n", temp_str);
		else 
			ok = 0;
	}
	
	strcat(new_line, "\n");
	fputs(new_line, temp);//fprintf(temp, "%s\n", new_line);
	
	while(fgets(acc_line, 100, fin) != NULL){
		strcat(acc_line, "");
		fputs(acc_line, temp);//fprintf(temp, "%s\n", temp_str);
	}
	
	fclose(fin);
	rewind(temp);
	
	fin = fopen("/home/cristi/project2/accounts.txt", "w");
	while(fgets(acc_line, 100, temp) != NULL){
		strcat(acc_line, "");
		fputs(acc_line, fin);//*/fprintf(fin, "%s\n", temp_str);
	}
	
	fclose(temp);
	fclose(fin);
}



















#endif
