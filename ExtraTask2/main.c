#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define MAX_DRIVERS 16
#define BUF_SIZE 128

enum DriverState {
    AVAILABLE,
    BUSY
};

typedef struct {
    pid_t pid;
    int to_driver[2];
    int from_driver[2];
    enum DriverState state;
    int waiting_for_status;
} Driver;

Driver drivers[MAX_DRIVERS];
int driver_count = 0;

void print_help() {
    printf("Available commands:\n");
    printf("  create_driver\n");
    printf("  send_task <pid> <task_timer>\n");
    printf("  get_status <pid>\n");
    printf("  get_drivers\n");
    printf("  help\n");
    printf("  exit\n");
}

void run_driver(int read_fd, int write_fd) {
    enum DriverState state = AVAILABLE;
    time_t busy_until = 0;

    char buf[BUF_SIZE];

    while (1) {
        ssize_t n = read(read_fd, buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';

        if (strncmp(buf, "send_task", 9) == 0) {
            int task_time;
            sscanf(buf + 10, "%d", &task_time);

            time_t now = time(NULL);
            if (state == BUSY && now < busy_until) {
                int remaining = (int)(busy_until - now);
                dprintf(write_fd, "Busy %d\n", remaining);
                continue;
            }

            state = BUSY;
            busy_until = now + task_time;

            pid_t child = fork();
            if (child == 0) {
                sleep(task_time);
                kill(getppid(), SIGUSR1);
                exit(0);
            }
            dprintf(write_fd, "OK\n");
        } else if (strncmp(buf, "get_status", 10) == 0) {
            time_t now = time(NULL);
            if (state == BUSY) {
                if (now >= busy_until) {
                    state = AVAILABLE;
                    dprintf(write_fd, "Available\n");
                } else {
                    int remaining = (int)(busy_until - now);
                    dprintf(write_fd, "Busy %d\n", remaining);
                }
            } else {
                dprintf(write_fd, "Available\n");
            }
        }
    }
}

void create_driver() {
    if (driver_count >= MAX_DRIVERS) {
        printf("Max drivers reached\n");
        return;
    }

    int to_driver_pipe[2];
    int from_driver_pipe[2];

    if (pipe(to_driver_pipe) < 0 || pipe(from_driver_pipe) < 0) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        close(to_driver_pipe[1]);
        close(from_driver_pipe[0]);

        run_driver(to_driver_pipe[0], from_driver_pipe[1]);
        exit(0);
    } 
    else {
        close(to_driver_pipe[0]);
        close(from_driver_pipe[1]);

        drivers[driver_count].pid = pid;
        drivers[driver_count].to_driver[1] = to_driver_pipe[1];
        drivers[driver_count].from_driver[0] = from_driver_pipe[0];
        drivers[driver_count].state = AVAILABLE;
        printf("Driver created with pid %d\n", pid);
        driver_count++;
    }
}

int find_driver_by_pid(pid_t pid) {
    for (int i = 0; i < driver_count; i++) {
        if (drivers[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

void send_task(pid_t pid, int task_time) {
    int idx = find_driver_by_pid(pid);

    if (idx < 0) {
        printf("No such driver\n");
        return;
    }

    char cmd[BUF_SIZE];
    snprintf(cmd, sizeof(cmd), "send_task %d\n", task_time);
    write(drivers[idx].to_driver[1], cmd, strlen(cmd));
}

void get_status(pid_t pid) {
    int idx = find_driver_by_pid(pid);

    if (idx < 0) {
        printf("No such driver\n");
        return;
    }

    drivers[idx].waiting_for_status = 1;
    char cmd[] = "get_status\n";
    write(drivers[idx].to_driver[1], cmd, strlen(cmd));
}

void get_drivers() {
    for (int i = 0; i < driver_count; i++) {
        get_status(drivers[i].pid);
    }
}

int main() {
    struct pollfd fds[MAX_DRIVERS + 1];
    char input_buf[BUF_SIZE];

    print_help();

    while (1) {
        printf("> ");
        fflush(stdout);
        
        fds[0].fd = 0;
        fds[0].events = POLLIN;

        for (int i = 0; i < driver_count; i++) {
            fds[i + 1].fd = drivers[i].from_driver[0];
            fds[i + 1].events = POLLIN;
        }

        int ret = poll(fds, driver_count + 1, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            ssize_t n = read(0, input_buf, sizeof(input_buf) - 1);
            if (n <= 0) {
                printf("EOF on stdin, exiting\n");
                break;
            }
            input_buf[n] = '\0';

            if (strncmp(input_buf, "create_driver", 13) == 0) {
                create_driver();
            } 
            else if (strncmp(input_buf, "send_task", 9) == 0) {
                pid_t pid;
                int task_time;
                if (sscanf(input_buf + 10, "%d %d", &pid, &task_time) == 2) {
                    send_task(pid, task_time);
                } 
                else {
                    printf("Usage: send_task <pid> <task_time>\n");
                }
            } 
            else if (strncmp(input_buf, "get_status", 10) == 0) {
                pid_t pid;
                if (sscanf(input_buf + 11, "%d", &pid) == 1) {
                    get_status(pid);
                } 
                else {
                    printf("Usage: get_status <pid>\n");
                }
            } 
            else if (strncmp(input_buf, "get_drivers", 11) == 0) {
                get_drivers();
            }
            else if (strncmp(input_buf, "help", 4) == 0) {
                print_help();
                continue;
            }
            else if (strncmp(input_buf, "exit", 4) == 0) {
                printf("Exiting...\n");
                break;
            }
            else {
                printf("Unknown command\n");
            }
        }

        for (int i = 0; i < driver_count; i++) {
            if (fds[i + 1].revents & POLLIN) {
                char resp[BUF_SIZE];
                ssize_t n = read(fds[i + 1].fd, resp, sizeof(resp) - 1);
                if (n > 0) {
                    resp[n] = '\0';
                    if (drivers[i].waiting_for_status) {
                        printf("Driver pid %d: %s", drivers[i].pid, resp);
                        drivers[i].waiting_for_status = 0;
                    } 
                    else {
                        printf("Response from driver %d: %s", drivers[i].pid, resp);
                    }
                }
            }
        }
    }

    for (int i = 0; i < driver_count; i++) {
        close(drivers[i].to_driver[1]);
        close(drivers[i].from_driver[0]);
        kill(drivers[i].pid, SIGKILL);
        waitpid(drivers[i].pid, NULL, 0);
    }

    return 0;
}
