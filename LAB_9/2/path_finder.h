#include <time.h>

typedef struct file_info {
    char files[100];
    long int file_size;
    char mod_time[18];
} file_info;

void formated_time(file_info* f_info, time_t mod_time);
void sort_files(file_info* f_info, int size);
char* normalize (char* path, char* new_path);
char* current_path (char* path);
int dir_files (file_info* f_info, char* path, char* new_path);