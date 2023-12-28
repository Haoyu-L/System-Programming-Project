#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

typedef struct {
    int ID;
    double requestNum[10];
} Request;

#define SERVER_SOCKET_PATH "RequestSocket"
#define MAX_LENGTH 100

int globalID = 0;

double generateRandomNumber(double min, double max){
    double ranNum = ((double)rand()) / RAND_MAX;
    return min + ranNum * (max - min);
}

int main(int argc, char* argv[]){

    int status, numRead, numWritten;
    int sfd;
    Request request;
    double response;
    struct sockaddr_un svaddr, claddr;

    if((argc != 2) || (strcmp(argv[1], "--help")) == 0 || (strcmp(argv[1], "-h") == 0)){
        printf("Usage: %s <ID>\n", argv[0]);
        exit(1);
    }

    request.ID = atoi(argv[1]);
    request.ID = getpid();
    globalID = request.ID;

    srand(time(NULL) + globalID);
    for (int i = 0; i < 10; i++){
        request.requestNum[i] = generateRandomNumber(0, 1);
    }

    // Create client socket
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        printf("client Failed to create socket.\n");
        exit(1);
    }

    // Bind client socket to unique pathname (based on PID)
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long) getpid());

    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1) {
        printf("client Failed to bind socket.\n");
        exit(1);
    }

    // Construct address of server
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

    // Send the request to the server
    numWritten = sendto(sfd, &request, sizeof(request), 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un));
    if (numWritten != sizeof(request)) {
        printf("client partial write error.\n");
        exit(1);
    }

    // Receive the response from the server
    numRead = recvfrom(sfd, &response, sizeof(double), 0, NULL, NULL);
    if (numRead != sizeof(double)) {
        printf("client Failed to read response.\n");
        exit(1);
    }

    printf("Random number: ");
    for(int n = 0; n < 10; n++){
        printf("%f, ",request.requestNum[n]);
    }
    printf("\n");
    printf("Average: %f\n", response);

    remove(claddr.sun_path);  // Remove client socket pathname

    exit(0);
}
