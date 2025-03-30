#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main (void) {
	char command[1024], c;
	char* args[64], *pc;
	int command_size, args_count;

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
		args_count = 0;

        	while (pc != NULL && args_count < 64) {
                	args[args_count++] = pc;
                	pc = strtok(NULL, " ");
        	}

        	args[args_count] = NULL;

        	pid_t pid = fork();

        	if (pid == -1) {
                	perror("Ошибка: не удалось породить процесс");
                	return -1;
        	}
        	else if (pid == 0) {
                	execvp(args[0], args);
                	perror("Ошибка: не удалось выполнить команду");
                	return -1;
        	}
        	else {
                	wait(NULL);
        	}
	}

	return 0;
}
