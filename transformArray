#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int computeSquare(int number)
{
    return number*number;
}

int computeCube(int number)
{
    return number*number*number;
}


void transformArrayOfNumbers(int* values, int count, int (*visitor)(int))
{
    /* TODO: Add arguments so that this can be called from main() */
    int index;
    for (index = 0; index < count; index++){
        values[index] = visitor(values[index]);
    }
    /*
        TODO:
        Apply a visitor (e.g., computeSquare or computeCube) to transform each element of an integer array
    */
}


int main(int argc, char* argv[])
{
    int* numbers;
    int numberCount;
    int index;

    /* 
        TODO:
        Handle command line arguments per the given requirements
    */
    if (argc < 3){
        printf("Usage: ./assignment1 <mode> <number_1> <number_2> ... <number_n>");
        exit(EXIT_FAILURE);
    }
    if ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 00) ){
        printf("Usage: ./assignment1 <mode> <number_1> <number_2> ... <number_n>");
        exit(EXIT_FAILURE);
    }

    /*else {
    printf("Invalid mode: %s\n", argv[1]);
    printf("Mode must be 'square' or 'cube'.\n");
    printf("Usage: ./assignment1 <mode> <number_1> <number_2> ... <number_n>\n");
    exit(EXIT_FAILURE);
    }*/



    numberCount = argc-2;/* TODO: calculate length of the array */
    numbers = (int*)malloc((numberCount)*sizeof(int)); /* TODO: allocate the array and check if it fails */
    if (numbers == NULL){
        printf("failed to allocate the memory");
        exit(EXIT_FAILURE);
    }

    for (index = 0; index < numberCount; index++) {
        if (isdigit(*argv[index+2])) {
            numbers[index] = atoi(argv[index+2]);
        } else {
            printf("Invalid number: %s\n", argv[index+2]);
            exit(EXIT_FAILURE);
        }
    }

    
    /*for (index = 0; index < numberCount; index++){
        numbers[index] = atoi(argv[index+1]);
    }*/
    
    /* Hints:
        (1) If the size of each integer is x bytes and the number of integers is y
        then the array needs x*y bytes.
        (2) malloc returns void*, therefore the (int*) casting is required.
    */

    /* 
        TODO:
        Populate the integer array based on command line arguments
    */

    if (strcmp(argv[1], "square") == 0)
    {
        transformArrayOfNumbers(
            numbers, 
            numberCount, 
            computeSquare);
    }

    else if (strcmp(argv[1], "cube") == 0)
    {
        transformArrayOfNumbers(
            numbers, 
            numberCount,
            computeCube);
    }

    else
    {
        printf("mode must be square or cube.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < numberCount; i++){
        printf("%d\n", numbers[i]);
    }
    //printf("%n\n", numbers);
    /*
        TODO:
        Print out the transformed integers, one integer per line
    */
    
    /* TODO: Deallocate the array */
    free(numbers);
    exit(EXIT_SUCCESS);
}
