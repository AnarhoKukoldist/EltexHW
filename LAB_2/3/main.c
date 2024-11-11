#include <stdio.h>

int main (void) {
	unsigned int size = 3;
	int matrix[size][size];

	int k1 = 1, k2 = 1;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < (size - k1); j++) {
			matrix[i][j] = 0;
		}
		for (int j = (size - 1); j > k2; j--) {
			matrix[i][j] = 1;
		}

		k1++;
		k2--;
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	return 0;
}
