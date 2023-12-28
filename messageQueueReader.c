#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
    int status;
    
    if((argc != 2) || (strcmp(argv[1], "--help")) == 0 || (strcmp(argv[1], "-h") == 0)){
        printf("Usage: %s <message queue name>\n", argv[0]);
        exit(1);
    }

    mqd_t messageQueueDescriptor;
    struct mq_attr attributes;

    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = sizeof(int);
    attributes.mq_curmsgs = 0;

    char messageQueueName[100];
    strcpy(messageQueueName, argv[1]);

    messageQueueDescriptor = mq_open(messageQueueName, O_RDONLY | O_NONBLOCK);
    if (messageQueueDescriptor == (mqd_t)-1) {
        printf("Failed to open message queue for reading");
        exit(1);
    }

    int number;
    status = mq_receive(messageQueueDescriptor, (char*)&number, sizeof(int), NULL);
    if (status == -1) {
        printf("Failed to read from message queue");
        exit(1);
    }
    while (status != -1){
        printf("%d\n", number);
        status = mq_receive(messageQueueDescriptor, (char*)&number, sizeof(int), NULL);
    }
    

    status = mq_close(messageQueueDescriptor);
    if (status == -1) {
        printf("Failed to close message queue");
        exit(1);
    }

    status = mq_unlink(messageQueueName);
    if (status == -1) {
        printf("Failed to unlink request message queue");
    }

    exit(0);
}
