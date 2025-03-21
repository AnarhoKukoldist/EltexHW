#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include "path_finder.h"

void formated_time(file_info* f_info, time_t mod_time) {
    struct tm *tm_info = localtime(&mod_time);
    
    if (!tm_info) {
        fprintf(stderr, "Ошибка получения времени\n");
        return;
    }

    strftime(f_info->mod_time, sizeof(f_info->mod_time), "%b %d %H:%M", tm_info);
}

void sort_files(file_info* f_info, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (f_info[j].files[0] == '/' && f_info[j + 1].files[0] != '/') {
                continue;
            }
            if (f_info[j].files[0] != '/' && f_info[j + 1].files[0] == '/') {
                file_info temp = f_info[j];
                f_info[j] = f_info[j + 1];
                f_info[j + 1] = temp;
                continue;
            }
            if (strcmp(f_info[j].files + 1, f_info[j + 1].files + 1) > 0) {
                file_info temp = f_info[j];
                f_info[j] = f_info[j + 1];
                f_info[j + 1] = temp;
            }
        }
    }
}

char* normalize (char* path, char* new_path) {
    if (!strcmp(path, "/")) {
        strcat(path, new_path + 1);
    }
    else if (!strcmp(new_path, "/..")) {
        int i = strlen(path) - 1;
        while (path[i] != '/') {
            i--;
        }
        path[i] = '\0';
        if (!strlen(path)) {
            path[0] = '/';
            path[1] = '\0';
        }   
    }
    else {
        strcat(path, new_path);
    }

    return path;
}

char* current_path (char* path) {
    if (!getcwd(path, 1024)) {
        exit(EXIT_FAILURE);
    }

    return path;
}

int dir_files (file_info* f_info, char* path, char* new_path) {
    if (*path != '/') {
        path = current_path(path);
    }
    if (*new_path == '/') {
        path = normalize(path, new_path);
    }

    DIR* dir = opendir(path);
    if (!dir) {
        exit(EXIT_FAILURE);
    }

    struct dirent* dirs;
    struct stat st;
    file_info* about_files = (file_info *)malloc(1800 * sizeof(file_info));
    int size = 0;

    while ((dirs = readdir(dir)) != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, dirs->d_name);
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                about_files[size].files[0] = '/';
            }
            else if (S_ISREG(st.st_mode) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
                about_files[size].files[0] = '*';
            }
            else if (S_ISREG(st.st_mode)) {
                about_files[size].files[0] = ' ';
            }

            int i = 1;
            for (int j = 0; j < (int)strlen(dirs->d_name); j++) {
                about_files[size].files[i] = dirs->d_name[j];
                i++;
            }
            about_files[size].files[i] = '\0';
            about_files[size].file_size = st.st_size;
            formated_time(about_files + size, st.st_ctime);
            size++;
        }
    }

    int pos = 0;

    for (int i = 0; i < size; i++) {
        if (!strcmp(about_files[i].files, "/.") || (!strcmp(about_files[i].files, "/..") && path[1] == '\0')) {
            continue;
        }
        if (about_files[i].files[0] == '/') {
            f_info[pos++] = about_files[i];
        }
    }
    for (int i = 0; i < size; i++) {
        if (about_files[i].files[0] == ' ' || about_files[i].files[0] == '*') {
            f_info[pos++] = about_files[i];
        }
    }

    sort_files(f_info, pos);

    closedir(dir);

    free(about_files);

    return pos;
}