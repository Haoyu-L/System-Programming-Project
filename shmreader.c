#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if((argc != 3) || (strcmp(argv[1], "--help")) == 0 || (strcmp(argv[1], "-h") == 0)){
        printf("Usage: %s <shared memory name> <semaphore name>\n", argv[0]);
        exit(1);
    }

    char* semName = argv[2];
    char* shmName = argv[1];

    int shmDescriptor;
    void *ptr;
    sem_t *mySemaphore;
    int status;
    struct stat shmMetadata;
    char* pointerToSharedVariable;
    int size;

    shmDescriptor = shm_open(shmName, O_RDONLY, 0);
    if (shmDescriptor == -1) {
        printf("Failed to open shared memory.\n");
        exit(1);
    }

    status = fstat(shmDescriptor, &shmMetadata);
    if (status == -1)
    {
        printf("Failed to get size of the shared memory.\n");
        exit(EXIT_FAILURE);
    }

    size = shmMetadata.st_size / 100;
    char stringToRead[size][100];

    ptr = mmap(NULL, shmMetadata.st_size, PROT_READ, MAP_SHARED, shmDescriptor, 0);
    if (ptr == MAP_FAILED) {
        printf("Failed to map shared memory into address space.\n");
        exit(1);
    }

    status = close(shmDescriptor);
    if (status == -1) {
        printf("Failed to close shared memory.\n");
        exit(1);
    }

    pointerToSharedVariable = (char*)ptr;

    mySemaphore = sem_open(semName, 0);
    if (mySemaphore == SEM_FAILED) {
        printf("Failed to open semaphore.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        status = sem_wait(mySemaphore);
        if (status != 0) {
            printf("Failed to wait on semaphore.\n");
            exit(1);
        }
        
        strcpy(stringToRead[i], pointerToSharedVariable + i * 100);

        printf("%s\n", stringToRead[i]);
        //printf("%s\n", pointerToSharedVariable + i * 100);

        status = sem_post(mySemaphore);
        if (status != 0) {
            printf("Failed to post semaphore.\n");
            exit(1);
        }
    }

    status = sem_close(mySemaphore);
    if (status != 0) {
        printf("Failed to close semaphore.\n");
        exit(1);
    }

    return 0;
}

