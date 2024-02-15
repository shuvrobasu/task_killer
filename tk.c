#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define CMD_SIZE 512
#else
#include <unistd.h>
#include <signal.h>
#define CMD_SIZE 1024
#endif

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void banner() {
    printf("Task Manager\n");
    printf("============\n");
    printf("88888888888               888           888    d8P  d8b 888 888\n");
    printf("   888                    888           888   d8P   Y8P 888 888\n");
    printf("   888                    888           888  d8P        888 888\n");
    printf("   888   8888b.  .d8888b  888  888      888d88K     888 888 888\n");
    printf("   888      88b  88K      888 .88P      8888888b    888 888 888\n");
    printf("   888  .d888888  Y8888b. 888888K       888  Y88b   888 888 888\n");
    printf("   888  888  888      X88 888  88b      888   Y88b  888 888 888\n");
    printf("   888  Y888888  88888P' 888   888      888    Y88b 888 888 888\n");
    printf("\n");
    printf("A simple task manager program to list and kill tasks.\n");
    printf("Developed by:Shuvro Basu.\n");
    printf("Version: 1.0\n"); printf("Check for updates: https://github.com/shuvro-basu/task_manager\n");
}

void listTasks() {
#ifdef _WIN32
    system("tasklist");
#else
    system("ps -A");
#endif
}

int killTaskByPID(int pid) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess != NULL) {
        if (TerminateProcess(hProcess, 0)) {
            CloseHandle(hProcess);
            return 1; // Success
        } else {
            DWORD errorCode = GetLastError();
            if (errorCode == ERROR_ACCESS_DENIED) {
                printf("Error: Access denied. Unable to kill process with PID %d\n", pid);
            } else {
                printf("Error: Unable to terminate process with PID %d\n", pid);
            }
            CloseHandle(hProcess);
            return 0; // Failed
        }
    } else {
        printf("Error: Unable to open process with PID %d\n, Check if PID is correct.", pid);
        return -1; // Error
    }
#else
    char cmd[CMD_SIZE];
    snprintf(cmd, CMD_SIZE, "sudo kill -9 %d", pid);
    int result = system(cmd);
    if (result == 0) {
        return 1; // Success
    } else {
        return 0; // Failed
    }
#endif
}

int killTaskByImageName(const char *imageName) {
#ifdef _WIN32
    char cmd[CMD_SIZE];
    snprintf(cmd, CMD_SIZE, "taskkill /IM \"%s\" /F", imageName);
    int result = system(cmd);
    if (result == 0) {
        return 1; // Success
    } else {
        return 0; // Failed
    }
#else
    printf("Error: Terminating a process by image name is not supported on Unix-based systems.\n");
    return -1; // Error
#endif
}

void printHelp() {
    printf("Usage: task_manager [options]\n");
    printf("Options:\n");
    printf("  -p <pid>        Kill a task by its process ID\n");
    printf("  -i <image_name> Kill a task by its image name\n");
    printf("  -h              Display this help message\n");
}

int main(int argc, char *argv[]) {
    clearScreen();
    banner();
    if (argc == 1) {
        printf("Listing all tasks:\n");
        listTasks();
        int pid;
        char input[36];
        printf("Enter the PID of the task you want to kill: ");
        fgets(input, sizeof(input), stdin);
        if (strcmp(input, "\n") == 0) {
            printf("Error: Blank PID entered. Exiting program.\n");
            return 1;
        }
        sscanf(input, "%d", &pid);
        printf("Killing task with PID %d...\n", pid);
        int result = killTaskByPID(pid);
        if (result == 1) {
            printf("Task with PID %d killed successfully.\n", pid);
        } else if (result == 0) {
            printf("Error 1: Task with PID %d not killed successfully.\n", pid);
            // No need to print anything here as killTaskByPID already printed the error
        }
        return 0;
    }

    if (strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "-p") == 0) {
        int pid = atoi(argv[2]);
        printf("Killing task with PID %d...\n", pid);
        int result = killTaskByPID(pid);
        if (result == 1) {
            printf("Task with PID %d killed successfully.\n", pid);
        } else if (result == 0) {
            // No need to print anything here as killTaskByPID already printed the error
        }
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "-i") == 0) {
        printf("Killing tasks with image name: %s...\n", argv[2]);
        int result = killTaskByImageName(argv[2]);
        if (result == 1) {
            printf("Tasks with image name '%s' killed successfully.\n", argv[2]);
        } else if (result == 0) {
            printf("Failed to kill tasks with image name '%s'.\n", argv[2]);
        }
        return 0;
    }

    printf("Error: Invalid arguments.\n");
    printHelp();
    return 1;
}
