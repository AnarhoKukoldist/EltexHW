#include <stdio.h>

void swap (int* a, int* b) {
	int c = *a;
	*a = *b;
	*b = c;
}

int main (void) {
	unsigned int n = 5;
	int array[n];

	for (int i = 0; i < n; i++) {
		scanf("%d", &array[i]);
	}

	int max = (n % 2) ? (n / 2) : (n / 2 + 1);

	for (int i = 0; i < max; i++) {
		swap(&array[i], &array[n - i - 1]);
	}

	for (int i = 0; i < n; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");

	return 0;
}
