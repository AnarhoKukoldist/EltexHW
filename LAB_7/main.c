#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "libcalc.h"

int numbers_to_array (char* buf, int* array) {
    int i = 0;

    if (fgets(buf, 70, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = ' ';
        }

        char* token = strtok(buf, " ");

        while (token != NULL) {
            if (strlen(token) > 1) {
                if (*token == '-' || isdigit(*token)) {
                    token++;
                }
                else {
                    printf("Ошибка: один из операндов - не число.\n\n");
                    i = 0;
                    break;
                }

                for (int j = 0; token[j] != '\0'; j++) {
                    if (!isdigit(token[j])) {
                        printf("Ошибка: один из операндов - не число.\n\n");
                        i = 0;
                        break;
                    }
                }
                token--;
            }
            else if (!isdigit(*token)) {
                printf("Ошибка: один из операндов - не число.\n\n");
                i = 0;
                break;
            }

            array[i] = atoi(token);
            token = strtok(NULL, " ");

            i++;
        }
    }

    return i;
}

int main (void) {
    int command, result;
    char buf[70];
    int array[70];

    printf("1) Сложение\n2) Вычитание\n3) Умножение\n4) Деление\n5) Выход\n\n");

    while (1) {
        printf("Введите номер команды: ");
        scanf("%d", &command);

        if (command == 5) {
            break;
        }
        else if (command < 0 || command > 5) {
            printf("Ошибка: неправильный номер команды.\n\n");
            continue;
        }

        printf("Введите целочисленные операнды через пробел:");
        getchar();
        printf("\n");

        int i = numbers_to_array(buf, array);

        if (i == 0) { // один из введённых операндов - не число
            continue;
        }

        if (i < 2) {
            printf("Ошибка: должно быть как минимум 2 операнда.\n\n");
            continue;
        }

        if (command == 1) {
            result = array[0];

            int j = 1;
            while(j < i) {
                result = add(result, array[j]);
                j++;
            }
        }
        else if (command == 2) {
            result = array[0];

            int j = 1;
            while(j < i) {
                result = sub(result, array[j]);
                j++;
            }
        }
        else if (command == 3) {
            result = array[0];

            int j = 1;
            while(j < i) {
                result = mul(result, array[j]);
                j++;
            }
        }
        else if (command == 4) {
            float result_div = (float)array[0];

            int j = 1;
            while(j < i) {
                result_div = div_f(result_div, (float)array[j]); // у функции такое название из-за конфликта с функцией из stdlib.h

                if (result_div == INFINITY) {
                    printf("Ошибка: деление на ноль.\n\n");
                    break;
                }

                j++;
            }

            printf("Результат: %.2f\n\n", result_div);
            continue;
        }

        printf("Результат: %d\n\n", result);
    }
}