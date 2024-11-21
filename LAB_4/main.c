#include <stdio.h>

struct abonent {
    char name[50];
    char second_name[50];
    char tel[50];
};

int free_index (const struct abonent* array, unsigned int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (array[i].name[0] == '0') {
            break;
        }
    }
    return i;
}

int main (void) {
    unsigned int size = 100;
    struct abonent array[size];

    for (int i = 0; i < size; i++) { // для упрощения заранее записываю нули
        array[i].name[0] = '0';
        array[i].second_name[0] = '0';
        array[i].tel[0] = '0';
    }

    printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n\n");

    unsigned int command = 0, index;
    char letter;
    while (command != 5) {
        printf("Введите команду: ");
        scanf("%d", &command);
        getchar(); // Добавил, чтобы строки "Введите имя: " и "Введите фамилию: " не вывелись одновременно
        printf("\n");

        if (command == 1) {
            index = free_index(array, size);
            if (index == size) {
                printf("Ошибка: абонентский справочник переполнен.\n");
                continue;
            }

            int j = 0;
            printf("Введите имя: ");
            while ((letter = getchar()) != '\n') {
                array[index].name[j] = letter;
                j++;
            }
                
            j = 0;
            printf("Введите фамилию: ");
            while ((letter = getchar()) != '\n') {
                array[index].second_name[j] = letter;
                j++;
            }

            j = 0;
            printf("Введите номер телефона: ");
            while ((letter = getchar()) != '\n') {
                array[index].tel[j] = letter;
                j++;
            }

            printf("\n");
        }
        else if (command == 2) {
            printf("Введите порядковый номер абонента: ");
            scanf("%d", &index);
            printf("\n");

            if (array[index].name[0] == '0') {
                printf("Ошибка: такого абонента не существует.\n\n");\
                continue;
            }

            for (int j = 0; j < 49; j++) {
                array[index].name[j] = '0';
                array[index].second_name[j] = '0';
                array[index].tel[j] = '0';
            }
        }
        else if (command == 3) {
            char name1[10];
            int j = 0;

            printf("Введите имя: ");
            while ((letter = getchar()) != '\n') {
                name1[j] = letter;
                j++;
            }
            printf("\n");

            int count;
            for (int i = 0; i < size; i++) {
                if (name1[0] == array[i].name[0]) {
                    count = 0;
                    for (j = 0; j < 10; j++) {
                        if (array[i].name[j] != name1[j]) {
                            break;
                        }
                        else {
                            count++;
                        }
                        if (count == 10) {
                            printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", array[i].name, array[i].second_name, array[i].tel);
                        }
                    }
                    
                }
            }
        }
        else if (command == 4) {
            for (int i = 0; i < size; i++) {
                if (array[i].name[0] != '0') {
                    printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", array[i].name, array[i].second_name, array[i].tel);
                }
            }
        }
        else if (command == 5) {
            break;
        }
        else {
            printf("Ошибка: неверная команда.\n");
        }
    }

    return 0;
}
