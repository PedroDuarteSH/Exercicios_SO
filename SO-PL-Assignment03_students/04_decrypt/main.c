#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define max_words 10
#define max_line 200

int getKey_list(char ***keylist);
char *decrypth(char *line, char ***keylist, int keylist_size, int line_number);
int *list_check(char letter, char ***keylist, int keylist_size, int letter_number);
int string_check(char *string, char ***keylist, int keylist_size);

int main(void) {

	char line[max_line];
	int result;
	FILE *file;
	//Get decryption data
	char ***keylist = malloc(max_words * sizeof(char **));
	int keylist_size = getKey_list(keylist);


	file = fopen("text.txt", "rt");
	char *newline;
	int line_number = 1;
		pid_t pid;
	while(result = fgets(line, max_line, file) != NULL && file!=NULL ){
		pid = fork();
		fflush(NULL);
		if(pid == 0){
			newline = decrypth(line, keylist, keylist_size, line_number);
			printf("line %d: %s", line_number, newline);
			_exit(0);
		}
		else{
			wait(NULL);
		}
		line_number++;
	}
	printf("\nTranslation complete. %d lines translated.\n", line_number-1);

}

char *decrypth(char *line, char ***keylist, int keylist_size, int line_number){

	char *newline = malloc(sizeof(char) * max_line);
	int newline_position = 0;
	int position = 0;
	int decrypt;

		while(line[position] != '\0'){
			decrypt = 0;
			int *possibilities = list_check(line[position], keylist, keylist_size, 0);
			for(int i = 0; i < keylist_size; i++){
				int index = possibilities[i];
				if(index != -1){
					if(keylist[index][1][strlen(keylist[index][1])-1] == line[position + strlen(keylist[index][1])-1]){
						char *string_to_verify = malloc(sizeof(char) * strlen(keylist[index][1] + 1));
						for (int i = 0; i < strlen(keylist[index][1]); i++) {
							string_to_verify[i] = line[position+i];
						}
						if(string_check(string_to_verify, keylist, keylist_size) == 1){
							string_to_verify = keylist[index][0];
							newline_position += strlen(keylist[index][0])-1;
							strcat(newline, string_to_verify);
							position += strlen(keylist[index][1])-1;
							decrypt = 1;
						}
					}
				}
			}
			if(decrypt == 0)
				newline[newline_position] = line[position];

			newline_position++;
			position++;
		}



	return newline;
}
int *list_check(char letter, char ***keylist, int keylist_size, int letter_number){
	int *possibilities = malloc(keylist_size * sizeof(int));
	for(int i = 0; i < keylist_size; i++){
		possibilities[i] = -1;
		if(keylist[i][1][letter_number] == letter){
			possibilities[i] = i;
		}

	}
	return possibilities;
}

int string_check(char *string, char ***keylist, int keylist_size){
	for(int i = 0; i < keylist_size; i++){
		if(strcmp (string, keylist[i][1]) == 0)	return 1;
	}
	return 0;
}


int getKey_list(char ***keylist){

	FILE *file = fopen("decrypt.txt", "rt");
	char line[200];
	int result;
	int i = 0;
	while(result = fgets(line, 200, file) != NULL){
		keylist[i] = malloc(2 * sizeof(char *));
		keylist[i][0] = malloc(max_line * sizeof(char));
		keylist[i][1] = malloc(max_line * sizeof(char));

		strcpy(keylist[i][0],strtok(line, " ")); //word decrypted
		strcpy(keylist[i][1],strtok(NULL, " ")); //word encripted
		keylist[i][1][strcspn(keylist[i][1], "\r\n")] = 0; //remove end \n if its there
		i++;
	}
	return i;
}
