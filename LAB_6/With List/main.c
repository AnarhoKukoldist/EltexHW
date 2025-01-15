#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct abonent {
    char name[50];
    char second_name[50];
    char tel[50];
} abonent;

typedef struct Node {
    abonent data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct List {
    Node* head;
    Node* tail;
    unsigned int size;
} List;

void init (List* list) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void push_back (List* list, char* _name, char* _second_name, char* _tel) {
    Node* current = (Node *)malloc(sizeof(Node));

    if (!current) {
        printf("Не удалось выделить память.\n");
        exit(0);
    }

    strcpy(current->data.name, _name);
    strcpy(current->data.second_name, _second_name);
    strcpy(current->data.tel, _tel);

    if (!list->head) {
        list->head = current;
        list->tail = current;
        list->head->prev = NULL;
        list->tail->next = NULL;
    }
    else {
        current->prev = list->tail;
        list->tail->next = current;
        list->tail = current;
        list->tail->next = NULL;
    }

    list->size++;
}

void pop_back (List* list) {
    if (list->size == 0) {
        printf("Ошибка: список пуст.\n");
        return;
    }

    printf("Удалена запись:\n");
    printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", list->tail->data.name, list->tail->data.second_name, list->tail->data.tel);

    Node* temp = list->tail;

    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    }
    else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }

    free(temp);
    list->size--;
}

void search (List* list, char* _name) {
    Node* temp = list->head;

    for (int i = 0; i < list->size; i++) {
        if (temp->data.name[0] == _name[0]) {
            int j = 1;
            while (strlen(temp->data.name) > j) {
                if (temp->data.name[j] == _name[j]) {
                    j++;
                }
                else {
                    break;
                }
            }
            if (strlen(temp->data.name) == j) {
                printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", temp->data.name, temp->data.second_name, temp->data.tel);
            }
        }
        temp = temp->next;
    }
}

void print (List* list) {
    if (list->size == 0) {
        printf("Ошибка: список пуст.\n\n");
        return;
    }

    Node* temp = list->head;

    while (temp != NULL) {
        printf("Имя: %s\nФамилия: %s\nНомер телефона: %s\n\n", temp->data.name, temp->data.second_name, temp->data.tel);
        temp = temp->next;
    }
}

void delete (List* list) {
    if (list->size == 0) {
        return;
    }
    else {
        Node* temp = list->head;

        while (temp != NULL) {
            Node* next = temp->next;
            free(temp);
            temp = next;
        }

        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

int main () {
    List list;
    init(&list);

    printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n\n");

    unsigned int command = 0;
    char letter;
    while (command != 5) {
        printf("Введите команду: ");
        scanf("%d", &command);
        getchar(); // Добавил, чтобы строки "Введите имя: " и "Введите фамилию: " не вывелись одновременно
        printf("\n");

        if (command == 1) {
            abonent current_abonent;

            int j = 0;
            printf("Введите имя: ");
            while ((letter = getchar()) != '\n' && (j < 50)) {
                current_abonent.name[j] = letter;
                j++;
            }
            current_abonent.name[j] = '\0';
                
            j = 0;
            printf("Введите фамилию: ");
            while ((letter = getchar()) != '\n' && (j < 50)) {
                current_abonent.second_name[j] = letter;
                j++;
            }
            current_abonent.second_name[j] = '\0';

            j = 0;
            printf("Введите номер телефона: ");
            while ((letter = getchar()) != '\n' && (j < 50)) {
                current_abonent.tel[j] = letter;
                j++;
            }
            current_abonent.tel[j] = '\0';
            printf("\n");

            push_back(&list, current_abonent.name, current_abonent.second_name, current_abonent.tel);
        }
        else if (command == 2) {
            pop_back(&list);
        }
        else if (command == 3) {
            char string[50];
            int j = 0;
            printf("Введите имя: ");
            while ((letter = getchar()) != '\n' && (j < 50)) {
                string[j] = letter;
                j++;
            }
            string[j] = '\0';
            printf("\n");

            search(&list, string);
        }
        else if (command == 4) {
            print(&list);
        }
        else if (command == 5) {
            delete(&list);
            break;
        }
        else {
            printf("Ошибка: неверная команда.\n");
        }
    }

    return 0;
}