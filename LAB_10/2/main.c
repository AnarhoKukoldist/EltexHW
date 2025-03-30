#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void create_process (int proc_num) {
	pid_t pid = fork();

	if (pid == -1) {
		perror("Ошибка: не удалось породить процесс");
		return;
	}
	else if (pid == 0) {
		printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
		if (proc_num == 1) {
			create_process(3);
			create_process(4);
		}
		else if (proc_num == 2) {
			create_process(5);
		}
		exit(0);
	}
	else {
		printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
		wait(NULL);
	}
}

int main (void) {
	create_process(1);
	create_process(2);

	return 0;
}
