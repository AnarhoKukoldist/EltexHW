#include <stdio.h>

void count_of_units (int number) {
	int count = 0;
	for (int i = 31; i >= 0; i--) {
		if (number & (1 << i)) {
			count++;
		}
	}

	printf("%d\n", count);
}

int main (void) {
	unsigned int number;
	scanf("%d", &number);

	count_of_units(number);

	return 0;
}
