#include <stdio.h>

int main (void) {
	unsigned int n;
	scanf("%d", &n);

	int array[n][n];

	int number = 1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			array[i][j] = number;
			number++;

			printf("%d ", array[i][j]);
		}
		printf("\n");
	}

	return 0;
}
