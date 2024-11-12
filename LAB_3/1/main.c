#include <stdio.h>

int main (void) {
	unsigned int number1, number2;

	scanf("%d %d", &number1, &number2);

	char* px1 = (char *)&number1; // создаю указатель на char, чтобы можно было сдвигаться на 1 байт вместо четырёх

	*(px1 + 2) = number2; // изменение третьего байта числа

	printf("%d\n", number1);

	return 0;
}
