#include <stdio.h>

void binary_print (int number) {
	for (int i = 31; i >= 0; i--) { // поскольку unsigned int равен 32 битам
		printf("%d", (number & (1 << i)) ? 1 : 0); // каждый раз сдвигаем единицу, чтобы проверить соответствующий бит

		if (i % 8 == 0) { // для красоты
			printf(" ");
		}
	}

	printf("\n");
}

int main (void) {
	unsigned int number; // чтобы были только положительныые числа
	scanf("%d", &number);

	binary_print(number);

	return 0;
}
