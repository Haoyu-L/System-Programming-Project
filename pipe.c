#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

typedef struct {
    char name[100];
    int count;
} myStruct;

int main(int argc, char *argv[]) {
    int pipeDescriptor[2];
    int status, childID, childStatus;
    int numRead, size;

    myStruct entry;

    if ((argc < 3) || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
        printf("Usage: %s <name 1> <count 1> <name 2> <count 2> ...\n", argv[0]);
        exit(1);
    }
    if (argc % 2 != 1) {
        printf("Usage: %s <name 1> <count 1> <name 2> <count 2> ...\n", argv[0]);
        exit(1);
    }

    status = pipe(pipeDescriptor);
    if (status != 0) {
        printf("Failed to create pipe1\n");
        exit(1);
    }

    childID = fork();
    switch (childID) {
        case -1:
            printf("Failed to create child process\n");
            exit(1);

        case 0:
            status = close(pipeDescriptor[1]);
            if (status == -1) {
                printf("Failed to close write descriptor\n");
                _exit(1);
            }

            numRead = read(pipeDescriptor[0], &entry, sizeof(myStruct));
            while (numRead > 0) {
                printf("Name: %s, count:%d\n", entry.name, entry.count);
                numRead = read(pipeDescriptor[0], &entry, sizeof(myStruct));
            }

            if (numRead == -1) {
                printf("Child Failed to read from pipe\n");
                _exit(1);
            }

            status = close(pipeDescriptor[0]);
            if (status == -1) {
                printf("Failed to close read descriptor\n");
                _exit(1);
            }

            _exit(0);

        default:
            status = close(pipeDescriptor[0]);
            if (status == -1) {
                printf("Parent Failed to close read descriptor\n");
                exit(1);
            }

            for (int i = 1; i < argc; i += 2) {
                myStruct entry;
                strcpy(entry.name, argv[i]);
                entry.count = atoi(argv[i + 1]);
                write(pipeDescriptor[1], &entry, sizeof(myStruct));
            }

            status = close(pipeDescriptor[1]);
            if (status == -1) {
                printf("Parent Failed to close write descriptor\n");
                exit(1);
            }

            status = wait(&childStatus);
            if (status == -1) {
                printf("Error in waiting for child to complete\n");
                exit(1);
            }

            exit(0);
    }
}

