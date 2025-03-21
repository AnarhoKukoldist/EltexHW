#include "path_finder.h"
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <string.h>
#include <sys/statvfs.h>
#include <locale.h>
#include <sys/wait.h>
#include <unistd.h>
//#include <wchar.h>

#define MAX_FILE_COUNT 1900

size_t utf8_strlen(const char *str) { // определение реального размера строки с возможными кириллическими символами

    size_t len = 0;
    while (*str) {
        int char_len = mblen(str, MB_CUR_MAX);
        if (char_len < 1) {
			break;
		}
        str += char_len;
        len++;
    }
    return len;
}

int main (void) {
	setlocale(LC_ALL, ""); // чтобы было реально вывести символы кириллицы

	initscr();
	refresh();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE); // цвета для окон и текста
	init_pair(2, COLOR_BLACK, COLOR_CYAN);
	init_pair(4, COLOR_WHITE, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(7, COLOR_GREEN, COLOR_BLUE);
	init_pair(9, COLOR_YELLOW, COLOR_BLUE);
	init_pair(10, COLOR_WHITE, COLOR_RED);
	init_color(COLOR_YELLOW, 1000, 700, 0);
	init_pair(6, COLOR_YELLOW, COLOR_BLUE);
	init_color(COLOR_MAGENTA, 0, 600, 800);
	init_pair(8, COLOR_MAGENTA, COLOR_BLUE);
	use_default_colors();
	init_pair(3, COLOR_WHITE, -1);

	char* string1[] = {"Left", "File", "Command", "Options", "Right"}; // заранее подготовленные строки для вставки
	char* string2 = "Hint: Mozhet byt' realizuyu pozzhe.";
	char* string3[] = {"Help", "Menu", "View", "Edit", "Copy", "RenMov", "Mkdir", "Delete", "PullDn", "Quit"};
	char string_for_bash[1024];

	char* path, path1[1024], path2[1024], new_path = '0'; // если '0', тогда перехода в новую директорию не будет
	path1[0] = '0'; // если первый символ '0', тогда путь path будет впервые найден после вызова dir_files
	path2[0] = '0';
	
	file_info* files1 = (file_info *)malloc(MAX_FILE_COUNT * sizeof(file_info)); // массивы структур для файлов, их размеров и даты последней модификации
	file_info* files2 = (file_info *)malloc(MAX_FILE_COUNT * sizeof(file_info));

	int file_count1 = dir_files(files1, path1, &new_path);
	int file_count2 = dir_files(files2, path2, &new_path);

	char username[256];
    char hostname[256];

	getlogin_r(username, sizeof(username)); // получение имени пользователя, причём строка получается длинее самого имени, поэтому при выводе приходится писать -8 для минимизации длины
	gethostname(hostname, sizeof(hostname)); // получение названия устройства

	int pos;
    for (int i = 0; i < (int)strlen(path1); i++) { // для правильного отображения пути в миниконсоли
		int count = 0;
		if (path1[i] == '/') {
			pos = i + 1;
			for (int j = 0; j < (int)strlen(username); j++) {
				if (username[j] == path1[pos]) {
					count++;
				}
				pos++;
			}
			if (count == (int)strlen(username)) {
				break;
			}
		}	
	}
	if (path1[pos] == '/') {
		strcat(string_for_bash + 1, path1 + pos);
		string_for_bash[0] = '~';
	}
	else {
		string_for_bash[0] = '~';
		string_for_bash[1] = '\0';
	}

	struct statvfs fs;
    
    if (statvfs("/", &fs) != 0) { // получение информации о памяти
        perror("statvfs");
        return 1;
    }

	long long total_size = fs.f_blocks * fs.f_frsize;
	long long occupied_size = total_size - fs.f_bfree * fs.f_frsize;
	total_size /= (1024 * 1024 * 1024);
	occupied_size /= (1024 * 1024 * 1024);

	int rows, cols;

	getmaxyx(stdscr, rows, cols); // получение соотношения сторон (далее будет вычисляться постоянно, чтобы подгонять масштаб mc)

	WINDOW *top_panel = newwin(1, cols, 0, 0);
	WINDOW *win_left = newwin(rows - 1 - 3, cols / 2, 1, 0);
	WINDOW *win_right = newwin(rows - 1 - 3, cols / 2 + (cols % 2 ? 1 : 0), 1, cols / 2);
	WINDOW *bash_and_buttons = newwin(3, cols, rows - 3, 0);
	
	cbreak(); // приятные приколы для ввода текста
	noecho();
    nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);

	char input[256] = "\0"; // строка для bash
	char temp_input[256] = "\0"; // доп строка для bash на тот случай, если пользователь захочет скоректировать строку через стрелки влево, вправо
	char temp_path[70] = "/home"; // вспомогательный массив для определения пути, который будет отображаться
	strcat(temp_path, username);
	int index = 0; // для навигации и заполнения строки, которая будет подана bash
	int c, c_prev = 0;
	int pos1 = 0; // для навигации в левом окне и
	int pos2 = 0; // правом окне
	int prev_pos1 = 0, prev_pos2 = -1; // для определения, в каком сейчас окне пользователь
	int min1 = 0, min2 = 0; // те позиции, с которых будет начинаться вывод файлов
	int max1 = rows - 9 < file_count1 ? rows - 9 : file_count1;
	int max2 = max1;

	while ((c = getch())) {
		getmaxyx(stdscr, rows, cols);

		if (c >= 32 && c <= 126) { // к сожалению, только с латинскими символами :(
			if (strlen(input) == index) {
				input[index++] = c;
				input[index] = '\0';
			}
			else {
				strcpy(temp_input, input + index);
				input[index++] = c;
				strcpy(input + index, temp_input);
			}
		}
		else if (c == KEY_BACKSPACE) {
			if (index > 0) {
				strcpy(temp_input, input + index);
				index--;
				strcpy(input + index, temp_input);
			}
		}
		else if (c == '\n') {
			if (c_prev >= 32 && c_prev <= 126) {
				if (!strcmp(input, "exit")) { // обработка команды exit
					break;
				}
				else { // обработка bash команд
					int i = 0;
					char* args[32];
					char* token = strtok(input, " ");
					
					while (token != NULL && i < 32) {
						args[i++] = token;
						token = strtok(NULL, " ");
					}
					args[i] = NULL;

					if (!strcmp(args[0], "cd")) { // обработка команды смены директории
						if (i != 2) {
							index = 0;
							input[index] = '\0';
							c_prev = c;
							continue;
						}
						if (!strcmp(args[1], "..")) { // переход в предыдущую директорию
							if (prev_pos1 != -1) {
								file_count1 = dir_files(files1, path1, files1[0].files);
								pos1 = 0;
								min1 = pos1;
								max1 = rows - 9 < file_count1 - pos1 ? rows - 9 : file_count1;
							}
							else {
								file_count2 = dir_files(files2, path2, files2[0].files);
								pos2 = 0;
								min2 = pos2;
								max2 = rows - 9 < file_count2 - pos2 ? rows - 9 : file_count2;
							}

							index = 0;
							input[index] = '\0';
							c_prev = c;
							continue;
						}
						else if (!strcmp(args[1], ".")) { // остаёмся в той же директории
							index = 0;
							input[index] = '\0';
							c_prev = c;
							continue;
						}
						else { // переход в конкретную директорию
							int coincidence = -1;
							int i;
							if (prev_pos1 != -1) { // проверка на существование такой директории и переход
								for (i = 0; i < file_count1; i++) {
									if ((coincidence = strcmp(args[1], files1[i].files + 1)) == 0) {
										break;
									}
								}
								if (!coincidence) {
									file_count1 = dir_files(files1, path1, files1[i].files);
									pos1 = 0;
									min1 = pos1;
									max1 = (rows - 9 + pos1) < (file_count1 - pos1) ? (rows - 9 + pos1) : file_count1;
								}
							}
							else {
								for (i = 0; i < file_count2; i++) {
									if ((coincidence = strcmp(args[1], files2[i].files + 1)) == 0) {
										break;
									}
								}
								if (!coincidence) {
									file_count2 = dir_files(files2, path2, files2[i].files);
									pos2 = 0;
									min2 = pos2;
									max2 = (rows - 9 + pos2) < (file_count2 - pos2) ? (rows - 9 + pos2) : file_count2;
								}
							}

							index = 0;
							input[index] = '\0';
							c_prev = c;
							continue;
						}
					}

					endwin();

					pid_t pid = fork(); // если не cd, тогда выполняем команду через новый процесс

					if (pid == -1) {
						perror("Не удалось породить процесс");
						exit(EXIT_FAILURE);
					}
					else if (pid == 0) {
						execvp(args[0], args); // выполняться будет в консоли linux
						perror("Не удалось выполнить команду");
						exit(EXIT_FAILURE);
					}
					else {
						waitpid(pid, NULL, 0);
						initscr(); // Возвращаем ncurses после завершения команды
        				refresh();
					}

					index = 0;
					input[index] = '\0'; // очищаем командную строку
				}
			}
			else if (c_prev == KEY_DOWN || c_prev == KEY_UP || index == 0) { // переход в директорию / nano / запуск исполняемого файла из правого / левого окна
				char* args[3];
				char* str_nano = "nano";
				char str_run[128] = "./";
				int dir_count;

				if (prev_pos1 != -1) {
					for (dir_count = 0; dir_count < file_count1; dir_count++) {
						if (files1[dir_count].files[0] != '/') {
							break;
						}
					}
					if (pos1 >= dir_count) { // nano / запуск исполняемого файла
						if (files1[pos1].files[0] == ' ') {
							args[0] = str_nano;
							args[1] = files1[pos1].files + 1;
							args[2] = NULL;
						}
						else {
							strcat(str_run, files1[pos1].files + 1);
							args[0] = str_run;
							args[1] = NULL;
							args[2] = NULL;
						}
					}
					else { // переход в директорию
						file_count1 = dir_files(files1, path1, files1[pos1].files);
						pos1 = 0;
						min1 = pos1;
						max1 = (rows - 9 + pos1) < (file_count1 - pos1) ? (rows - 9 + pos1) : file_count1;
						continue;
					}
				}
				else {
					for (dir_count = 0; dir_count < file_count2; dir_count++) {
						if (files2[dir_count].files[0] != '/') {
							break;
						}
					}

					if (pos2 >= dir_count) {
						if (files2[pos2].files[0] == ' ') {
							args[0] = str_nano;
							args[1] = files2[pos2].files + 1;
							args[2] = NULL;
						}
						else {
							strcpy(str_run, files2[pos2].files + 1);
							args[0] = str_run;
							args[1] = NULL;
							args[2] = NULL;
						}
					}
					else {
						file_count2 = dir_files(files2, path2, files2[pos2].files);
						pos2 = 0;
						min2 = pos2;
						max2 = (rows - 9 + pos2) < (file_count2 - pos2) ? (rows - 9 + pos2) : file_count2;
						continue;
					}
				}

				endwin();

				pid_t pid = fork(); // порождение процесса для запуска nano / исполняемого файла

				if (pid == -1) {
					perror("Не удалось породить процесс");
					exit(EXIT_FAILURE);
				}
				else if (pid == 0) {
					execvp(args[0], args);
					perror("Не удалось выполнить команду");
					exit(EXIT_FAILURE);
				}
				else {
					waitpid(pid, NULL, 0);
					initscr(); // Возвращаем ncurses после завершения команды
        			refresh();
				}
			}
		}
		if (c == '\t') { // перемещение между окнами на TAB
			if (prev_pos1 != -1) {
				prev_pos1 = -1;
				prev_pos2 = 0;
			}
			else {
				prev_pos1 = 0;
				prev_pos2 = -1;
			}
		}
		if (c == KEY_DOWN) {
			if (prev_pos1 != -1) {
				if (pos1 < max1 - 1) {
					pos1++;
				}
				else if (max1 < file_count1 && (pos1 == max1 - 1)) {
					pos1++;
					min1 = pos1;
					max1 = ((rows - 9) + pos1) < (file_count1) ? (rows - 9 + pos1) : file_count1;
				}
			}
			else {
				if (pos2 < max2 - 1) {
					pos2++;
				}
				else if (max2 < file_count2 && (pos2 == max2 - 1)) {
					pos2++;
					min2 = pos2;
					max2 = ((rows - 9) + pos2) < (file_count2) ? (rows - 9 + pos2) : file_count2;
				}
			}
		}
		else if (c == KEY_UP) {
			if (prev_pos1 != -1) {
				if (pos1 > min1) {
					pos1--;
				}
				else if (min1 > 0 && (pos1 == min1)) {
					pos1--;
					max1 = pos1 + 1;
					min1 = (pos1 - (rows - 9)) > 0 ? (pos1 - (rows - 9) + 1) : 0;
				}
			}
			else {
				if (pos2 > min2) {
					pos2--;
				}
				else if (min2 > 0 && (pos2 == min2)) {
					pos2--;
					max2 = pos2 + 1;
					min2 = (pos2 - (rows - 9)) > 0 ? (pos2 - (rows - 9) + 1) : 0;
				}
			}

		}
		else if (c == KEY_LEFT) {
			if (index) {
				index--;
			}
		}
		else if (c == KEY_RIGHT) {
			if (index != strlen(input)) {
				index++;
			}
		}
		if (c != ERR) { // с = ERR + объяснение этого в конце while
			c_prev = c; 
		}

		werase(top_panel);
		werase(win_left);
		werase(win_right);
		werase(bash_and_buttons);

		int start_y = 0;
		int start_x = 2;

		WINDOW *top_panel = newwin(1, cols, 0, 0);
		wbkgd(top_panel, COLOR_PAIR(2));
		wattron(top_panel, COLOR_PAIR(2));

		for (int i = 0; i < 5; i++) {
			mvwprintw(top_panel, start_y, start_x, "%s", string1[i]);
			if (sizeof(string1) + 2 + 4 * 5 > cols) {
				start_x += 3 + strlen(string1[i]);
			}
			else {
				start_x += 4 + strlen(string1[i]);
			}
		
		}

		WINDOW *win_left = newwin(rows - 1 - 3, cols / 2, 1, 0);
		wbkgd(win_left, COLOR_PAIR(1));
		wattron(win_left, COLOR_PAIR(1));
		box(win_left, 0, 0);

		mvwprintw(win_left, 0, 1, "<");
		mvwprintw(win_left, 0, cols / 2 - 6, ".[^]>");

		int place = 0;
		for (int i = min1; i < max1; i++) {
			if (i == pos1 && (prev_pos1 != -1)) {
				wattron(win_left, COLOR_PAIR(2));
				mvwprintw(win_left, place + 2, 1, "%s", files1[i].files);

				if (!strcmp(files1[i].files, "/..")) {
					mvwprintw(win_left, place + 2, cols / 2 - 21, "%s", "UP--DIR");
				}
				else {
					mvwprintw(win_left, place + 2, cols / 2 - 21, "%ld", files1[i].file_size);
				}
				
				mvwprintw(win_left, place + 2, cols / 2 - 13, "%s", files1[i].mod_time);
				wattroff(win_left, COLOR_PAIR(2));
				place++;
				continue;
			}
			if (files1[i].files[0] == '*') {
				wattron(win_left, COLOR_PAIR(7));
				mvwprintw(win_left, place + 2, 1, "%s", files1[i].files);
				mvwprintw(win_left, place + 2, cols / 2 - 21, "%ld", files1[i].file_size);
				mvwprintw(win_left, place + 2, cols / 2 - 13, "%s", files1[i].mod_time);
				wattroff(win_left, COLOR_PAIR(7));
			}
			else if ((files1[i].files[0] == '/') || (files1[i].files[0] == ' ' && ((files1[i].files[strlen(files1[i].files) - 2] == '.') || (files1[i].files[1] == '.') || (files1[i].files[strlen(files1[i].files) - 2] != '.')))) {
				mvwprintw(win_left, place + 2, 1, "%s", files1[i].files);
				
				if (!strcmp(files1[i].files, "/..")) {
					mvwprintw(win_left, place + 2, cols / 2 - 21, "%s", "UP--DIR");
				}
				else {
					mvwprintw(win_left, place + 2, cols / 2 - 21, "%ld", files1[i].file_size);
				}
				
				mvwprintw(win_left, place + 2, cols / 2 - 13, "%s", files1[i].mod_time);
			}
			else if (files1[i].files[0] == ' ' && ((files1[i].files[strlen(files1[i].files) - 2] == '.' && files1[i].files[strlen(files1[i].files) - 1] == 'c') || files1[i].files[strlen(files1[i].files) - 1] == 'h')) {
				wattron(win_left, COLOR_PAIR(8));
				mvwprintw(win_left, place + 2, 1, "%s", files1[i].files);
				mvwprintw(win_left, place + 2, cols / 2 - 21, "%ld", files1[i].file_size);
				mvwprintw(win_left, place + 2, cols / 2 - 13, "%s", files1[i].mod_time);
				wattroff(win_left, COLOR_PAIR(8));
			}
			else {
				wattron(win_left, COLOR_PAIR(9));
				mvwprintw(win_left, place + 2, 1, "%s", files1[i].files);
				mvwprintw(win_left, place + 2, cols / 2 - 21, "%ld", files1[i].file_size);
				mvwprintw(win_left, place + 2, cols / 2 - 13, "%s", files1[i].mod_time);
				wattroff(win_left, COLOR_PAIR(9));
			}
			place++;
		}

		wattron(win_left, COLOR_PAIR(6));
		mvwprintw(win_left, 1, 1, ".n");
		mvwprintw(win_left, 1, (cols / 2 - 24) / 2, "Name");
		mvwprintw(win_left, 1, cols / 2 - 21, "Size");
		mvwprintw(win_left, 1, cols / 2 - 13, "Modify time");
		wattroff(win_left, COLOR_PAIR(6));

		mvwhline(win_left, rows - 7, 1, ACS_HLINE, cols / 2 - 2);
		mvwaddch(win_left, rows - 7, 0, ACS_LTEE);
		mvwaddch(win_left, rows - 7, cols / 2 - 1, ACS_RTEE);
		mvwvline(win_left, 1, cols / 2 - 22, ACS_VLINE, rows - 8);
		mvwvline(win_left, 1, cols / 2 - 14, ACS_VLINE, rows - 8);

		if (pos1 == 0) {
			mvwprintw(win_left, rows - 6, 1, "UP--DIR");
		}
		else {
			mvwprintw(win_left, rows - 6, 1, "%s", files1[pos1].files);
		}

		mvwprintw(win_left, rows - 5, cols / 2 - 19, " %lldG / %lldG (%lld%%) ", occupied_size, total_size, (occupied_size * 100) / total_size);

		WINDOW *win_right = newwin(rows - 1 - 3, cols / 2 + (cols % 2 ? 1 : 0), 1, cols / 2);
		wbkgd(win_right, COLOR_PAIR(1));
		wattron(win_right, COLOR_PAIR(1));
		box(win_right, 0, 0);

		mvwprintw(win_right, 0, 1, "<");
		mvwprintw(win_right, 0, cols / 2 + (cols % 2 ? 1 : 0) - 6, ".[^]>");

		place = 0;
		for (int i = min2; i < max2; i++) {
			if (i == pos2 && (prev_pos2 != -1)) {
				wattron(win_right, COLOR_PAIR(2));
				mvwprintw(win_right, place + 2, 1, "%s", files2[i].files);

				if (!strcmp(files2[i].files, "/..")) {
					mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%s", "UP--DIR");
				}
				else {
					mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%ld", files2[i].file_size);
				}

				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 13, "%s", files2[i].mod_time);
				wattroff(win_right, COLOR_PAIR(2));
				place++;
				continue;
			}
			if (files2[i].files[0] == '*') {
				wattron(win_right, COLOR_PAIR(7));
				mvwprintw(win_right, place + 2, 1, "%s", files2[i].files);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%ld", files2[i].file_size);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 13, "%s", files2[i].mod_time);
				wattroff(win_right, COLOR_PAIR(7));
			}
			else if ((files2[i].files[0] == '/') || (files2[i].files[0] == ' ' && ((files2[i].files[strlen(files2[i].files) - 2] == '.') || (!strcmp(files2[i].files, " .")) || (files2[i].files[strlen(files2[i].files) - 2] != '.')))) {
				mvwprintw(win_right, place + 2, 1, "%s", files2[i].files);

				if (!strcmp(files2[i].files, "/..")) {
					mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%s", "UP--DIR");
				}
				else {
					mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%ld", files2[i].file_size);
				}

				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 13, "%s", files2[i].mod_time);
			}
			else if (files2[i].files[0] == ' ' && ((files2[i].files[strlen(files2[i].files) - 2] == '.' && files2[i].files[strlen(files2[i].files) - 1] == 'c') || files2[i].files[strlen(files2[i].files) - 1] == 'h')) {
				wattron(win_right, COLOR_PAIR(8));
				mvwprintw(win_right, place + 2, 1, "%s", files2[i].files);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%ld", files2[i].file_size);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 13, "%s", files2[i].mod_time);
				wattroff(win_right, COLOR_PAIR(8));
			}
			else {
				wattron(win_right, COLOR_PAIR(9));
				mvwprintw(win_right, place + 2, 1, "%s", files2[i].files);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 21, "%ld", files2[i].file_size);
				mvwprintw(win_right, place + 2, cols / 2 + (cols % 2 ? 1 : 0) - 13, "%s", files2[i].mod_time);
				wattroff(win_right, COLOR_PAIR(9));
			}
			place++;
		}

		wattron(win_right, COLOR_PAIR(6));
		mvwprintw(win_right, 1, 1, ".n");
		mvwprintw(win_right, 1, (cols / 2 + (cols % 2 ? 1 : 0) - 24) / 2, "Name");
		mvwprintw(win_right, 1, cols / 2 + (cols % 2 ? 1 : 0) - 21, "Size");
		mvwprintw(win_right, 1, cols / 2 + (cols % 2 ? 1 : 0) - 13, "Modify time");
		wattroff(win_right, COLOR_PAIR(6));

		mvwhline(win_right, rows - 7, 1, ACS_HLINE, cols / 2 - 1);
		mvwaddch(win_right, rows - 7, 0, ACS_LTEE);
		mvwaddch(win_right, rows - 7, cols / 2 + (cols % 2 ? 1 : 0) - 1, ACS_RTEE);
		mvwvline(win_right, 1, cols / 2 + (cols % 2 ? 1 : 0) - 22, ACS_VLINE, rows - 8);
		mvwvline(win_right, 1, cols / 2 + (cols % 2 ? 1 : 0) - 14, ACS_VLINE, rows - 8);

		if (pos2 == 0) {
			mvwprintw(win_right, rows - 6, 1, "UP--DIR");
		}
		else {
			mvwprintw(win_right, rows - 6, 1, "%s", files2[pos2].files);
		}

		mvwprintw(win_right, rows - 5, cols / 2 + (cols % 2 ? 1 : 0) - 19, " %lldG / %lldG (%lld%%) ", occupied_size, total_size, (occupied_size * 100) / total_size);

		WINDOW *bash_and_buttons = newwin(3, cols, rows - 3, 0);
		wbkgd(bash_and_buttons, COLOR_PAIR(3));
		wattron(bash_and_buttons, COLOR_PAIR(3));

		int j = 0;

		for (int i = 0; i < 10; i++) {
			wattron(bash_and_buttons, COLOR_PAIR(4));
			if (i + 1 == 10) {
				mvwprintw(bash_and_buttons, 2, j, "%d", i + 1);
			}
			else {
				mvwprintw(bash_and_buttons, 2, j, " %d", i + 1);
			}
			wattroff(bash_and_buttons, COLOR_PAIR(4));
			j += 2;
			wattron(bash_and_buttons, COLOR_PAIR(2));
			mvwprintw(bash_and_buttons, 2, j, "%s", string3[i]);
			int k;
			if (strlen(string3[i]) < 7) {
				int half1 = cols / 2 - 10;
				int half2 = cols - 10 + (cols % 2 ? 1 : 0);
				int max = half1 / 5;
				if (i + 1 == 5) {
					max = half1 + 10 - j;
				}
				if (i + 1 == 10) {
					max = half2 + 10 - j;
				}
			
				for (k = strlen(string3[i]); k < max; k++) {
					mvwprintw(bash_and_buttons, 2, j + k, " ");
				}
			}
			wattroff(bash_and_buttons, COLOR_PAIR(2));
			j += k;
		}

		wattron(bash_and_buttons, COLOR_PAIR(1));
		mvwprintw(bash_and_buttons, 1, cols - 3, "[^]");
		wattroff(bash_and_buttons, COLOR_PAIR(1));

		if (prev_pos1 != -1) {
			path = path1;
		}
		else {
			path = path2;
		}

		for (int i = 0; i < (int)strlen(path); i++) {
			int count = 0;
			if (path[i] == '/') {
				pos = i + 1;
				for (int j = 0; j < (int)strlen(username); j++) {
					if (username[j] == path[pos]) {
						count++;
					}
					pos++;
					
				}
				if (count == (int)strlen(username)) {
					break;
				}
			}	
		}

		if (utf8_strlen(temp_path) < strlen(path)) {
			strcpy(string_for_bash + 1, path + pos);
			string_for_bash[0] = '~';
		}
		else if (utf8_strlen(temp_path) == utf8_strlen(path)) {
			string_for_bash[0] = '~';
			string_for_bash[1] = '\0';
		}
		else {
			if (!strcmp(path, "/home")) {
				strcpy(string_for_bash, "/home");
				string_for_bash[5] = '\0';
			}
			else if (!strcmp(path, "/")) {
				string_for_bash[0] = '/';
				string_for_bash[1] = '\0';
			}
			else {
				strcpy(string_for_bash, path);
			}
			
		}

		size_t len_path = utf8_strlen(string_for_bash);
		mvwprintw(bash_and_buttons, 0, 0, "%s", string2);
		mvwprintw(bash_and_buttons, 1, 0, "%s@", username);
		mvwprintw(bash_and_buttons, 1, strlen(username) + 1, "%s:", hostname);
		mvwprintw(bash_and_buttons, 1, strlen(username) + strlen(hostname) + 2, "%s$ ", string_for_bash);
		mvwprintw(bash_and_buttons, 1, strlen(username) + strlen(hostname) + len_path + 4, "%s", input);
		wmove(bash_and_buttons, 1, strlen(username) + strlen(hostname) + len_path + 4 + index);

		wrefresh(top_panel);
		wrefresh(win_left);
		wrefresh(win_right);
		wrefresh(bash_and_buttons);

		c = ERR; // чтобы не было постонного повторение одного и того же символа

		napms(20);
	}

	delwin(top_panel);
	delwin(win_left);
	delwin(win_right);
	delwin(bash_and_buttons);
	endwin();

	free(files1);
	free(files2);

	return 0;
}
