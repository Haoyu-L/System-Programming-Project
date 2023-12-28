#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char *argv[]) {
    if((argc < 4) || (strcmp(argv[1], "--help")) == 0 || (strcmp(argv[1], "-h") == 0)){
        printf("Usage: %s <shared memory name> <semaphore name> <s1> <s2> ...\n", argv[0]);
        exit(1);
    }
    

    int size = argc - 3;
    char* semName = argv[2];
    char* shmName = argv[1];

    int shmDescriptor;
    void *ptr;
    sem_t *mySemaphore;
    int status;

    shmDescriptor = shm_open(shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(shmDescriptor == -1){
        printf("Failed to create shared memory.\n");
        exit(1);
    }

    status = ftruncate(shmDescriptor, size * 100);
    if(status == -1){
        printf("Failed to set the size of the shared memory.\n");
        exit(1);
    }

    ptr = mmap(0, size * 100, PROT_WRITE, MAP_SHARED, shmDescriptor, 0);
    if(ptr == MAP_FAILED){
        printf("Failed to map shared memory into address space.\n");
        exit(1);
    }

    status = close(shmDescriptor);
    if(status != 0){
        printf("Failed to close shared memory.\n");
        exit(1);
    }

    mySemaphore = sem_open(semName, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(mySemaphore == SEM_FAILED){
        printf("Failed to create semaphore.\n");
        exit(1);
    }

    char* pointerToSharedVariable = (char*)ptr;
    char stringToWrite[size][100];

    for (int i = 0; i < size; i++) {
        status = sem_wait(mySemaphore);
        if(status != 0){
            printf("Failed to wait on semaphore.\n");
            exit(1);
        }
        
        strcpy(stringToWrite[i], argv[i + 3]);
        strcpy(pointerToSharedVariable + i * 100, stringToWrite[i]);
        


        status = sem_post(mySemaphore);
        if(status != 0){
            printf("Failed to post on semaphore.\n");
            exit(1);
        }
    }

    status = sem_close(mySemaphore);
    if (status != 0)
    {
        printf("Failed to close semaphore.\n");
        exit(1);
    }

    exit(0);
}
