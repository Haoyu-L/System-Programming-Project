#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){

    int processNum = argc-1;
    pid_t childProcessId[processNum];
    int status;

    if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Usage: ./proc <n_1> <n_2> ...<n_k>\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < processNum; i++) {

        childProcessId[i] = fork();
        int sleepTime = atoi(argv[i+1]);
        if(sleepTime <0){
            printf("Invalid sleep time.\n");
            exit(1);
        }

        switch(childProcessId[i]) {
            case -1:
                printf("failed to create child.\n");
                exit(1);
            
            case 0:
                sleep(sleepTime);
                //printf("Child process %d completed.\n", i+1);
                //printf("child process %d is sleeping for %d seconds.\n", i+1, sleepTime);
                _exit(0);
            
            default:
                printf("Creating child process %d.\n", i+1);
        }
    }

    for (int i = processNum; i > 0; i--){
        waitpid(childProcessId[i-1], &status, 0);
        printf("Child process %d completed.\n", i);
    }
    exit(0);

}
