#include <stdio.h>

int strlen (const char* string) {
	int i = 0;
	while (string[i] != '\0') {
		i++;
	}

	return i;
}

char* pointer_find (char* string, char* substring) {
	if (strlen(string) < strlen(substring)) {
                printf("Ошибка: подстрока не может быть больше, чем строка.\n");
                return NULL;
        }

	int i, j;
        for (i = 0; i < strlen(string); i++) {
                if (*(string + i) == *substring) {
                        int match = 1;
                        for (j = 1; j < strlen(substring); j++) {
                                if (string[i + j] == substring[j]) {
                                        match++;
                                } else {
					break;
				}

                                if (match == strlen(substring)) {
                                        return string + i;
                                }
                        }
                }
        }

        return NULL;
}

int main (void) {
	char c, string[50], substring[50];
	int i = 0;

	while ((c = getchar()) != '\n') {
		*(string + i) = c;
		i++;
	}

	i = 0;

	while ((c = getchar()) != '\n') {
                *(substring + i) = c;
                i++;
        }

	char* pc = pointer_find(string, substring);

	printf("%p\n", pc);

	return 0;
}
