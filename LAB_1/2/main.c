// всё тоже самое, что и в первом задании
#include <stdio.h>

void binary_print (int number) {
	for (int i = 31; i >= 0; i--) {
		printf("%d", (number & (1 << i)) ? 1 : 0);

		if (i % 8 == 0) {
			printf(" ");
		}
	}

	printf("\n");
}

int main (void) {
	int number; // разница только в unsigned int и int
	scanf("%d", &number);

	if (number >= 0) {
		printf("Error: number must be negative.\n");
		return 1;
	}

	binary_print(number);

	return 0;
}
