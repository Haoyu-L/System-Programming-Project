#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

volatile int globalVariable = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct myStruct{
    int time;
    int number;
};

void* threadFunction1(void* input){
    struct myStruct threadInput = *(struct myStruct*)input;
    int status;

    for(int i = 0; i < threadInput.time; i++){
        status = pthread_mutex_lock(&mutex);
        if(status != 0){
            printf("Locking mutex error\n");
            exit(1);
        }

        globalVariable = globalVariable + threadInput.number;
           

        status = pthread_mutex_unlock(&mutex);
        if(status != 0){
            printf("Unlocking mutex error\n");
            exit(1);
        }

    }
    return NULL;
}

void* threadFunction2(void* input){
    struct myStruct threadInput = *(struct myStruct*)input;
    int status;

    for(int i = 0; i < threadInput.time; i++){
        status = pthread_mutex_lock(&mutex);
        if(status != 0){
            printf("Locking mutex error\n");
            exit(1);
        }

        globalVariable = globalVariable - threadInput.number;
           

        status = pthread_mutex_unlock(&mutex);
        if(status != 0){
            printf("Unlocking mutex error\n");
            exit(1);
        }

    }
    return NULL;
}



int main (int argc, char* argv[]){
    pthread_t thread1, thread2;
    int status;
    void* result;
    struct myStruct threadInput1;
    struct myStruct threadInput2;

    if((argc != 5) || (atoi(argv[1]) <= 0)|| (atoi(argv[2]) <= 0)|| (atoi(argv[3]) <= 0) || (atoi(argv[4]) <= 0) || 
    (strcmp(argv[1], "--help")) == 0 || (strcmp(argv[1], "-h") == 0)){
        printf("Usage: ./threads <n1> <s1> <n2> <s2>\n");
        exit(1);
    }

    threadInput1.time = atoi(argv[1]);
    threadInput1.number = atoi(argv[2]);
    threadInput2.time = atoi(argv[3]);
    threadInput2.number = atoi(argv[4]);
    
    status = pthread_create(&thread1, NULL, threadFunction1, (void*)&threadInput1);
    if(status != 0){
        printf("Creating thread 1 error\n");
        exit(1);
    }

    status = pthread_create(&thread2, NULL, threadFunction2, (void*)&threadInput2);
    if(status != 0){
        printf("Creating thread 2 error\n");
        exit(1);
    }

    status = pthread_join(thread1, &result);
    if(status != 0){
        printf("Joining thread 1 error\n");
        exit(1);
    }

    status = pthread_join(thread2, &result);
    if(status != 0){
        printf("Joining thread 2 error\n");
        exit(1);
    }

    printf("Final value of globalVariable = %d\n", globalVariable);

    return 0;
}
