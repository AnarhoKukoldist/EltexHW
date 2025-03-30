#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main (void) {
	int status;
	pid_t pid = fork();

	if (pid == -1) {
		perror("Ошибка: не удалось породить процесс");
		return -1;
	}
	else if (pid == 0) {
		printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
		exit(0);
	}
	else {
		printf("Родительствкий процесс: PID = %d, PPID = %d\n", getpid(), getppid());
		wait(&status);
		printf("Статус завершения ребёнка = %d\n", status);
	}

	return 0;
}
