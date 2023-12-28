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
    if(argc < 4 || argc % 2 != 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
        printf("Usage: %s <message queue name> <n1> <p1> <n2> <p2> ...\n", argv[0]);
        exit(1);
    }

    int status;
    mqd_t messageQueueDescriptor;
    struct mq_attr attributes;

    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = sizeof(int);
    attributes.mq_curmsgs = 0;

    char messageQueueName[100];
    strcpy(messageQueueName, argv[1]);

    messageQueueDescriptor = mq_open(messageQueueName, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes);
    if (messageQueueDescriptor == (mqd_t)-1) {
        printf("Writer failed to create/open message queue");
        exit(1);
    }

    int number = 0;
    int priority = 0;
    int count = (argc - 2) / 2;
    for (int i = 0; i < count; i++){
        number = atoi(argv[2 * i + 2]);
        priority = atoi(argv[2 * i + 3]);
        status = mq_send(messageQueueDescriptor, (char*)&number, sizeof(int), priority);
        if (status == -1) {
            printf("Failed to write to message queue");
            exit(1);
        }
    }

    status = mq_close(messageQueueDescriptor);
    if (status == -1) {
        printf("Writer failed to close message queue");
        exit(1);
    }

    exit(0);
}
