#include <stdio.h>

int main (void) {
	FILE* file = fopen("output.txt", "w+");

	if (!file) {
		printf("Ошибка: не удалось создать файл.\n");
		return -1;
	}

	fprintf(file, "String from file");

	fseek(file, 0, SEEK_END);
	int index = ftell(file);

	char c;
	while (index > 0) {
		fseek(file, --index, SEEK_SET);

		fscanf(file, "%c", &c);
		printf("%c", c);

	}
	printf("\n");

	fclose(file);

	return 0;
}