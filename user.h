#ifndef user_h
#define user_h

#include "utils.h"

struct User{
	int id;
	char username[20];
	char password[15];
};

struct User create_user(int input_id, char *input_username, char *input_password){
	struct User user;
	user.id = input_id;
	strcpy(user.username, input_username);
	strcpy(user.password, input_password);
	return user;
}

void print_user(struct User user){
	printf("User ID: %d\n", user.id);
	printf("Username: %s\n", user.username);
	printf("Password: %s", user.password);
}

void user_to_string(struct User user, char *res){
	char id_str[10];

	strcat(res, itoa(user.id, id_str));
	strcat(res, "|");
	strcat(res, user.username);
	strcat(res, "|");
	strcat(res, user.password);
	strcat(res, "|");
}

struct User user_from_line(char *user_line){
	char **fields = split_by_sep(user_line, '|', 3);
	struct User user = create_user(atoi(fields[0]), fields[1], fields[2]);
	free(fields);
	return user;
}

struct User get_user_by_id(int input_id){
	FILE *fin;
	fin = fopen("/home/cristi/project2/users.txt", "r");
	char user_line[50];
	struct User ghost_user;
	
	while(fgets(user_line, 50, fin)){
		ghost_user = user_from_line(user_line);
		if(ghost_user.id == input_id)
			fclose(fin);
			return ghost_user;
	}
	
	fclose(fin);
	
	struct User not_found = create_user(-1, "", "");
	return not_found;
}

struct User get_by_username(char *input_username){
	FILE *fin;
	fin = fopen("/home/cristi/project2/users.txt", "r");
	char user_line[50];
	struct User ghost_user;
	
	while(fgets(user_line, 50, fin)){
		ghost_user = user_from_line(user_line);
		if(equal_strings(ghost_user.username, input_username)){
			fclose(fin);
			return ghost_user;
		}
	}
	
	fclose(fin);
	
	struct User not_found = create_user(-1, "", "");
	return not_found;
}

int get_last_user_id(){
	FILE *fin;
	fin = fopen("/home/cristi/project2/users.txt", "r");
	char user_line[50];
	struct User ghost_user;
	
	while(fgets(user_line, 50, fin)){
		ghost_user = user_from_line(user_line);
	}
	
	fclose(fin);
	
	return ghost_user.id;
}

void sign_up(char *input_username, char *input_password){
	struct User new_user = create_user(get_last_user_id() + 1, input_username, input_password);
	char new_user_str[50] = "";
	user_to_string(new_user, new_user_str);
	
	FILE *fin;
	fin = fopen("/home/cristi/project2/users.txt", "a");
	fprintf(fin, "%s\n", new_user_str);
	fclose(fin);
}











#endif
