#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_DRIVERS 100

typedef enum { AVAILABLE, BUSY } State;

typedef struct {
    pid_t pid;
    int to_driver;
    int from_driver;
} Driver;

Driver drivers[MAX_DRIVERS];
int driver_count = 0;

// Логика драйвера
volatile State state = AVAILABLE;
volatile time_t busy_until = 0;
int write_fd = -1;

void handle_available(int sig) {
    state = AVAILABLE;
    if (write_fd != -1) {
        dprintf(write_fd, "Available\n");
    }   
}

void run_driver(int read_fd, int w_fd) {
    write_fd = w_fd;
    signal(SIGUSR1, handle_available);

    char buf[128];

    while (1) {
        ssize_t n = read(read_fd, buf, sizeof(buf) - 1);

        if (n <= 0) {
            break;
        }

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
        } 
        else if (strncmp(buf, "get_status", 10) == 0) {
            time_t now = time(NULL);
            if (state == BUSY) {
                if (now >= busy_until) {
                    state = AVAILABLE;
                    dprintf(write_fd, "Available\n");
                } 
                else {
                    int remaining = (int)(busy_until - now);
                    dprintf(write_fd, "Busy %d\n", remaining);
                }
            } 
            else {
                dprintf(write_fd, "Available\n");
            }
        }
    }
}

void create_driver() {
    if (driver_count >= MAX_DRIVERS) {
        printf("Maximum number of drivers reached.\n");
        return;
    }

    int to_driver[2];
    int from_driver[2];
    pipe(to_driver);
    pipe(from_driver);

    pid_t pid = fork();
    
    if (pid == 0) {
        close(to_driver[1]);
        close(from_driver[0]);
        run_driver(to_driver[0], from_driver[1]);
        exit(0);
    }

    close(to_driver[0]);
    close(from_driver[1]);

    drivers[driver_count].pid = pid;
    drivers[driver_count].to_driver = to_driver[1];
    drivers[driver_count].from_driver = from_driver[0];
    driver_count++;

    printf("Created driver with PID %d\n", pid);
}

Driver* find_driver(pid_t pid) {
    for (int i = 0; i < driver_count; i++) {
        if (drivers[i].pid == pid) {
            return &drivers[i];
        }
    }
    return NULL;
}

void send_task(pid_t pid, int task_time) {
    Driver* d = find_driver(pid);
    if (!d) {
        printf("Driver %d not found\n", pid);
        return;
    }

    dprintf(d->to_driver, "send_task %d\n", task_time);

    char buf[128];
    ssize_t n = read(d->from_driver, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Driver %d: %s", pid, buf);
    }
}

void get_status(pid_t pid) {
    Driver* d = find_driver(pid);
    if (!d) {
        printf("Driver %d not found\n", pid);
        return;
    }

    dprintf(d->to_driver, "get_status\n");

    char buf[128];
    ssize_t n = read(d->from_driver, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Driver %d: %s", pid, buf);
    }
}

void get_drivers() {
    for (int i = 0; i < driver_count; i++) {
        printf("Driver %d: ", drivers[i].pid);
        dprintf(drivers[i].to_driver, "get_status\n");

        char buf[128];
        ssize_t n = read(drivers[i].from_driver, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);
        } 
        else {
            printf("No response\n");
        }
    }
}

int main() {
    char cmd[128];

    printf("Taxi CLI started. Commands: create_driver, send_task <pid> <seconds>, get_status <pid>, get_drivers\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(cmd, sizeof(cmd), stdin)) {
            break;
        }

        if (strncmp(cmd, "create_driver", 13) == 0) {
            create_driver();
        } 
        else if (strncmp(cmd, "send_task", 9) == 0) {
            pid_t pid;
            int sec;
            if (sscanf(cmd + 10, "%d %d", &pid, &sec) == 2) {
                send_task(pid, sec);
            } else {
                printf("Usage: send_task <pid> <seconds>\n");
            }
        } 
        else if (strncmp(cmd, "get_status", 10) == 0) {
            pid_t pid;
            if (sscanf(cmd + 11, "%d", &pid) == 1) {
                get_status(pid);
            } 
            else {
                printf("Usage: get_status <pid>\n");
            }
        } 
        else if (strncmp(cmd, "get_drivers", 11) == 0) {
            get_drivers();
        } 
        else {
            printf("Unknown command\n");
        }
    }

    return 0;
}
