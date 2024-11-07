#include <stdio.h>

void print_bytes (int number) {
        for (int i = 31; i >= 0; i--) {
                printf("%d", (number & (1 << i)) ? 1 : 0);

                if (i % 8 == 0) {
                        printf(" ");
                }
        }

        printf("\n");
}

void changed_byte (int number, int number1) {
	number = number & ~(0xFF << 16); // Тут происходит зануление 3го байта, где 0xFF - байт единиц
	number = number | (number1 << 16); // Сдвигаем второе число и вставляем в первое

	print_bytes(number);
}

int main (void) {
	unsigned int number, number1;
	scanf("%d %d", &number, &number1);

	if (number1 > 511) { // Второе число должно умещаться в один байт (512 - один байт)
		return 1;
	}

	changed_byte(number, number1);

	return 0;
}
