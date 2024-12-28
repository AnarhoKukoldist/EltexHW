#include <stdio.h>
#include <stdlib.h>

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

struct abonent* add_abonent (struct abonent* array, unsigned int size) {
    char letter;

    if (size == 0) {
        array = malloc(sizeof(struct abonent));
    }
    else {
        array = realloc(array, (size + 1) * sizeof(struct abonent));
    }

    if (!array) {
        printf("Ошибка: не удалось выделить память.\n");
        exit(1);
    }

    int i = 0;
    printf("Введите имя: ");
    while ((letter = getchar()) != '\n') {
        array[size].name[i] = letter;
        i++;
    }
                
    i = 0;
    printf("Введите фамилию: ");
    while ((letter = getchar()) != '\n') {
        array[size].second_name[i] = letter;
        i++;
    }

    i = 0;
    printf("Введите номер телефона: ");
    while ((letter = getchar()) != '\n') {
        array[size].tel[i] = letter;
        i++;
    }

    printf("\n");

    return array;
}

struct abonent* delete_abonent (struct abonent* array, unsigned int index, unsigned int size) {
    if (size == 1) {
        free(array);
        return array;
    }

    while (index < (size - 1)) { // решил реализовать так (перетащить каждый последующий элемент после index назад, и перевыделить память)
        int i = 0;
        while (array[index + 1].name[i] != '\0') {
            array[index].name[i] = array[index + 1].name[i];
            i++;
        }
        array[index].name[i] = '\0';

        i = 0;
        while (array[index + 1].second_name[i] != '\0') {
            array[index].second_name[i] = array[index + 1].second_name[i];
            i++;
        }
        array[index].second_name[i] = '\0';

        i = 0;
        while (array[index + 1].tel[i] != '\0') {
            array[index].tel[i] = array[index + 1].tel[i];
            i++;
        }
        array[index].tel[i] = '\0';

        index++;
    }

    array = realloc(array, (size - 1) * sizeof(struct abonent));

    if (!array) {
        printf("Ошибка: не удалось выделить память.\n");
        exit(1);
    }

    return array;
}

void search_abonent (const struct abonent* array, unsigned int size) {
    char name1[10], letter;
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
            count = 1;
            for (int k = 1; k < j; k++) {
                if (array[i].name[k] != name1[k]) {
                    break;
                }
                else {
                    count++;
                }
                if (count == j) {
                    printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", array[i].name, array[i].second_name, array[i].tel);
                }
            }       
        }
    }
}

void print_abonent (const struct abonent* array, unsigned int size) {
    for (int i = 0; i < size; i++) {
        printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", array[i].name, array[i].second_name, array[i].tel);
    }
}



int main (void) {
    unsigned int size = 0;
    struct abonent* array = NULL;

    printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n\n");

    unsigned int command = 0;
    while (command != 5) {
        printf("Введите команду: ");
        scanf("%d", &command);
        getchar(); // Добавил, чтобы строки "Введите имя: " и "Введите фамилию: " не вывелись одновременно
        printf("\n");

        if (command == 1) {
            array = add_abonent(array, size);
            size++;
        }
        else if (command == 2) {
            if (size == 0) {
                printf("Ошибка: в списке нет абонентов.\n\n");
                continue;
            }

            unsigned int index;

            printf("Введите порядковый номер абонента: ");
            scanf("%d", &index);
            printf("\n");

            if (index < 0 || index > (size - 1)) {
                printf("Ошибка: некорректный порядковый номер абонента.\n\n");\
                continue;
            }

            array = delete_abonent(array, index, size);
            size--;
        }
        else if (command == 3) {
            search_abonent(array, size);
        }
        else if (command == 4) {
            print_abonent(array, size);
        }
        else if (command == 5) {
            free(array);
            break;
        }
        else {
            printf("Ошибка: неверная команда.\n");
        }
    }

    return 0;
}