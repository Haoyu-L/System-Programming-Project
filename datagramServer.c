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
    struct sockaddr_un claddr;
    int sfd;

    Request request = *(Request*)requestPointer;

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

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%d", request.ID);

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        printf("server Failed to create socket.\n");
        exit(1);
    }

    numWritten = sendto(sfd, &response, sizeof(double), 0, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un));
    if (numWritten != sizeof(double)) {
        printf("server partial write error.\n");
        exit(1);
    }

    close(sfd);

    free(requestPointer);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    int status, numRead;
    int sfd;
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

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
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

    while (TRUE){
        len = sizeof(struct sockaddr_un);
        numRead = recvfrom(sfd, &request, sizeof(Request), 0, (struct sockaddr *) &claddr, &len);
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
