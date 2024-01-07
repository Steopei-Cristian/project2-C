#ifndef utils_h
#define utils_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

char *itoa(int nr, char *str){
	if(str == NULL)
	return NULL;

	sprintf(str, "%d", nr);  
	return str;
}

char *ftoa(float nr, char *str){
	if(str == NULL)
	return NULL;

	sprintf(str, "%.2f", nr);  
	return str;
}

int equal_strings(char *str1, char *str2){
	int l1 = strlen(str1);
	int l2 = strlen(str2);
	
	if(l1 != l2)
		return 0;
		
	for(int i = 0; i < l1; i++)
		if(str1[i] != str2[i])
			return 0;
	
	return 1;
}

char **split_by_sep(char *string, char sep, int count){
	char **words = (char**)calloc(count, sizeof(char*));
	for(int i = 0; i < count; i++)
		words[i] = (char*)calloc(100, sizeof(char));

	char current[100];
	int d = strlen(string), k = 0, ct = 0;
	
	for(int i = 0; i < d && ct < count; i++){
		if(string[i] == sep || i == d - 1){
			current[k] = '\0';
			strcpy(words[ct++], current);
			k = 0;
		}
		else
			current[k++] = string[i];
	}
	
	return words;	
}

char* get_dollar_balance(float balance, char *res){
	strcpy(res, "");
	char balance_str[25];
	strcat(res, ftoa(balance, balance_str));
	strcat(res, " $");
	return res;
	
}

time_t convert_to_date(char *date) {
    struct tm tm_date = {0};
    sscanf(date, "%d.%d.%d", &tm_date.tm_mday, &tm_date.tm_mon, &tm_date.tm_year);
    tm_date.tm_mon--; // Adjusting month to be 0-indexed (0-11)
    tm_date.tm_year -= 1900; // Adjusting year to be years since 1900

    return mktime(&tm_date);
}

int is_in_interval(char *date, char *start, char *end) {
    time_t date_time = convert_to_date(date);
    time_t start_time = convert_to_date(start);
    time_t end_time = convert_to_date(end);

    return (date_time >= start_time && date_time <= end_time);
}



















#endif
