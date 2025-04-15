#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main (void) {
    char command[1024], c;
	char* args[64][10], *pc;
	int command_size, args_count = 0;

	while (1) {
		printf("Введите команду: ");

		command_size = 0;

        while ((c = getchar()) != '\n') {
            command[command_size++] = c;
        }
        command[command_size] = '\0';

        if (!strlen(command)) {
            perror("Ошибка: команда слишком короткая");
            continue;
        }
		if (!strcmp(command, "exit")) {
			break;
		}

        pc = strtok(command, " ");
        int i = 0;

        while (1) {
            if (pc == NULL) {
                args[args_count++][i] = NULL;
                break;
            }
            else if (*pc == '|') {
                args[args_count++][i] = NULL;
                i = 0;
            }
            else {
                args[args_count][i++] = pc; 
            }

            pc = strtok(NULL, " ");
        }

        int pipes[args_count - 1][2];

        for (int i = 0; i < args_count - 1; i++) {
            pipe(pipes[i]);
        }

        pid_t pids[args_count];

        for (int i = 0; i < args_count; i++) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("Ошибка: не удалось породить процесс");
                return 1;
            }
            else if (pid == 0) {
                if (i < args_count - 1) { // текущая команда - не последняя
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                if (i > 0) { // текущая команда - не первая
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }

                for (int j = 0; j < args_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execvp(args[i][0], args[i]);
                perror("Ошибка: не удалось выполнить команду");
                exit(0);
            }
            else {
                pids[i] = pid;
            }
        }

        for (int i = 0; i < args_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        for (int i = 0; i < args_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}