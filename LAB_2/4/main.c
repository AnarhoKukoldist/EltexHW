#include <stdio.h>

int main (void) {
	unsigned int size = 5;
	int matrix[size][size];

	int k1 = size, k2 = size, k3 = -1, k4 = 0, k5 = 0; // начальные значения, чтобы заполнять матрицу с нужных позиций (k5), и не выходить за границы (k1, k2, k3, k4)
	int n = 1;
	int right, down, left, up;
	int max = (size % 2) ? (size / 2 + 1) : (size / 2); // количество циклов для чётного/нечётного размера матрицы

	for (int i = 0; i < max; i++) {
		for (right = k5; right < k1; right++) {
			matrix[i][right] = n;
			n++;
		}
		k5++;
		right--;
		for (down = k5; down < k2; down++) {
			matrix[down][right] = n;
			n++;
		}
		k5 = size - k5 - 1;
		down--;
		for (left = k5; left > k3; left--) {
			matrix[down][left] = n;
			n++;
		}
		left++;
		for (up = k5; up > k4; up--) {
			matrix[up][left] = n;
			n++;
		}
		k5 = size - k5 - 1;
		up++;
		k1--;
		k2--;
		k3++;
		k4++;
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	return 0;
}
