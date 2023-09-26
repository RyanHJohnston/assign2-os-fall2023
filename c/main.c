#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define DOUBLE_MIN 0.0
#define DOUBLE_MAX 1.0


/****************** HELPER API ******************************/
int is_numeric(char *str);
double generate_random_double(double min, double max);



/****************** DRIVER **********************************/
int 
main(int argc, char *argv[]) 
{
    
    // n will be given as command line argument
    // struct timespec ts_begin, ts_end
    // double elapsed
    int i;
    int base;
    char *endptr;
    char *str;
    long n;
    size_t A_lengh;
    struct timespec ts_begin;
    struct timespec ts_end;

    // Dynamic arrays
    double elapsed;
    double *A;
    double *B;
    double *C;
    double *A_first_half;
    double *A_second_half;
    
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "ERROR: One integer parameter is needed\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    str = argv[1];
    base = (argc > 2) ? atoi(argv[2]) : 0;
    
    errno = 0;
    n = strtol(str, &endptr, base);
    

    // check for varoius possible errors
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) {
        fprintf(stderr, "ERROR: No digits were found\n");
        exit(EXIT_FAILURE);
    }
    
    // if reached here, strtol successfully parsed a number
    if (*endptr != '\0') {
        fprintf(stdout, "Further characters after number: \"%s\"\n", endptr);
    }

    // number has to be even for my own sanity, might change later to take odd numbers
    if (n % 2 != 0) {
        fprintf(stderr, "ERROR: Parameter must be an even integer\n");
        exit(EXIT_FAILURE);
    }

    // Allocate the arrays
    A_lengh = sizeof(double) * n;
    fprintf(stdout, "%ld\n", A_lengh);
    A = (double*)malloc(A_lengh);
    fprintf(stdout, "%ld\n", A_lengh / 2);
    B = (double*)malloc(A_lengh);
    C = (double*)malloc(A_lengh);
    A_first_half = (double*)malloc( A_lengh / 2);
    A_second_half = (double*)malloc( A_lengh / 2);

    srand((unsigned int)time(NULL)); // init rand seed, ensure rand values on different runs

    // create an array A (n double values) and randomly generate these values
    for (i = 0; i < n; ++i) {
        A[i] = generate_random_double(DOUBLE_MIN, DOUBLE_MAX);
    }
    
    // ------- ONE THREAD CASE ------------
    // copy A into B
    




    free(A);
    free(B);
    free(C);
    free(A_first_half);
    free(A_second_half);

    return 0;
}


/****************** HELPER API DEFINITIONS ******************/
int 
is_numeric(char *str) 
{
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 1;  // If any character is not a digit, return false
        }
        str++;  // Move to the next character
    }
    return 0;;
}

double generate_random_double(double min, double max) 
{
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

