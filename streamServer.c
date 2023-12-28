#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

typedef struct {
    int ID;
    double requestNum[10];
    int cfd;
} Request;

#define TRUE 1
#define FALSE 0

#define SERVER_SOCKET_PATH "RequestSocket"
#define MAX_LENGTH 100

pthread_mutex_t nextTicketMutex = PTHREAD_MUTEX_INITIALIZER;

void removeRequestSocket(void){
    int status;
    status = unlink(SERVER_SOCKET_PATH);
    if (status == -1){
        printf("Failed to unlink request socket.\n");
    }
}

void handleInterruptSignal(int){
    exit(0);
}

void* giveOutTickets(void* requestPointer){
    double response, sum = 0;
    int numWritten, status;
    int cfd;

    Request request = *(Request*)requestPointer;

    cfd = request.cfd;

    status = pthread_mutex_lock(&nextTicketMutex);
    if (status != 0){
        printf("Failed to lock mutex.\n");
        exit(1);
    }

    for (int i = 0; i < 10; i++){
        sum += request.requestNum[i];
    }
    response = sum / 10;

    status = pthread_mutex_unlock(&nextTicketMutex);
    if (status != 0){
        printf("Failed to unlock mutex.\n");
        exit(1);
    }

    numWritten = write(cfd, &response, sizeof(double));
    if (numWritten != sizeof(double)) {
        printf("server partial write error.\n");
        exit(1);
    }

    close(cfd);

    free(requestPointer);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    int status, numRead;
    int sfd, cfd;
    Request request;
    Request* threadInput;
    pthread_t threadId;
    struct sockaddr_un svaddr, claddr;
    socklen_t len;

    //register exit handler to delete the request socket
    status = atexit(removeRequestSocket);
    if (status != 0){
        printf("server failed to register exit handler.\n");
        exit(1);
    }

    //register CRRL+C signal handler to delete the request socket
    if (signal(SIGINT, handleInterruptSignal) == SIG_ERR){
        printf("server failed to register CTRL C signal handler.\n");
        exit(1);
    }

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        printf("server Failed to create socket.\n");
        exit(1);
    }

    if (remove(SERVER_SOCKET_PATH) == -1 && errno != ENOENT) {
        printf("server Failed to remove socket.\n");
        exit(1);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1) {
        printf("server Failed to bind socket.\n");
        exit(1);
    }

    if (listen(sfd, 5) == -1)
        //errExit("listen");
        printf("Failed to listen.\n");

    while (TRUE){
        len = sizeof(struct sockaddr_un);
        cfd = accept(sfd, (struct sockaddr *) &claddr, &len);
        if (cfd == -1)
            //errExit("accept");
            printf("Failed to accept connection.\n");
            

        numRead = read(cfd, &request, sizeof(Request));
        if (numRead != sizeof(Request)) {
            printf("server Failed to read request.\n");
            exit(1);
        }

        //create a brand new request object just for this thread
        threadInput = (Request*)malloc(sizeof(Request));
        if (threadInput == NULL){
            printf("Failed to allocate memory.\n");
            exit(1);
        }

        memcpy(threadInput, &request, sizeof(Request));
        threadInput->cfd = cfd;

        status = pthread_create(&threadId, NULL, giveOutTickets, (void*)threadInput);
        if (status != 0){
            printf("Failed to create thread.\n");
            exit(1);
        }

        status = pthread_detach(threadId);
        if(status != 0){
            printf("Failed to detach thread.\n");
            exit(1);
        }

    }
    exit(0);
}
