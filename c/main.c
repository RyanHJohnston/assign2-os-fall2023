#include <bits/time.h>
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
#define COPY_ARRAY_INDEX 0

typedef struct ThreadData {
    double *array;
    size_t length;
    double *avg;
} ThreadData;

/****************** HELPER FUNCTIONS ******************************/
/* Basic Help Functions */
void print_array(double *array, int length);
int is_numeric(char *str);
double generate_random_double(double min, double max);
void copy_array(double *src, double *dest, int length, int index);

/* Thread Functions */
void *sort_thread_avg(void *arg);



/****************** DRIVER **********************************/
int 
main(int argc, char *argv[]) 
{
    
    // n will be given as command line argument
    // struct timespec ts_begin, ts_end
    // double elapsed
    int i;
    int base;
    double *avg;
    char *endptr;
    char *str;
    long n;
    size_t A_byte_size;
    struct timespec ts_begin;
    struct timespec ts_end;

    // thread IDs
    pthread_t tid_1;

    // Dynamic arrays
    double elapsed;
    double *A;
    double *B;
    double *C;
    double *A_first_half;
    double *A_second_half;
    
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "ERROR: One integer parameter is needed\n");
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
    A_byte_size = sizeof(double) * n;
    A = (double*)malloc(A_byte_size);
    B = (double*)malloc(A_byte_size);
    C = (double*)malloc(A_byte_size);
    A_first_half = (double*)malloc( A_byte_size / 2);
    A_second_half = (double*)malloc( A_byte_size / 2);

    srand((unsigned int)time(NULL)); // init rand seed, ensure rand values on different runs

    // create an array A (n double values) and randomly generate these values
    for (i = 0; i < n; ++i) {
        A[i] = generate_random_double(DOUBLE_MIN, DOUBLE_MAX);
    }
    

    /********************* ONE THREAD CASE *************************/
    // copy A into B
    copy_array(A, B, n, COPY_ARRAY_INDEX);
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);
    ThreadData *B_data = (ThreadData*)malloc(sizeof(ThreadData));
    B_data->array = B;
    B_data->length = n;
    
    /*
     * HERE YOU ARE GETTING THE CORRECT AVERAGE BUT IT IS NOT RETURNING THE SORTED ARRAY
     * DONE! I NEEDED TO WORK WITH THE ARRAY STRUCT
     */
    pthread_create(&tid_1, NULL, sort_thread_avg, (void *)B_data);
    pthread_join(tid_1, (void **)&B_data);

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsed = ts_end.tv_sec - ts_begin.tv_sec;
    elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;

    /* Sorting is done in 10.0ms when two threads are used */
    /* Print average and at least first 10 values of the sorted array */
    if (n < 10) {
        fprintf(stdout, "Average of sorted array: %f\n", *B_data->avg);
        print_array(B_data->array, n);
    } else {
        fprintf(stdout, "Average of sorted array: %f\n", *B_data->avg);
        print_array(B_data->array, 10);
    }
    

    /*********** TWO THREADS CASE *********************/


    free(A);
    free(B);
    free(C);
    free(A_first_half);
    free(A_second_half);

    return 0;
}


/****************** HELPER API DEFINITIONS ******************/
void
print_array(double *array, int length)
{
    int i;
    for (i = 0; i < length; ++i) {
        fprintf(stdout, "%f\n", array[i]);
    }
}


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

double 
generate_random_double(double min, double max) 
{
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

/*
 * Copies values of src array to dest array
 * Index parameter must always be zero
 */
void 
copy_array(double *src, double *dest, int length, int index)
{
    if (index < length) {
        dest[index] = src[index];
        copy_array(src, dest, length, ++index);
    }
}

static void 
swap(double* xp, double* yp) 
{
    double temp = *xp;
    *xp = *yp;
    *yp = temp;
}

static void 
selection_sort(double *array, int n)
{
    int i;
    int j;
    int min_index;

    // one by one move the boundary of the unsorted array
    for (i = 0; i < n - 1; ++i) {
        // find the minimum element in the unsorted array
        min_index = i;
        for (j = i + 1; j < n; ++j) {
            if (array[j] < array[min_index]) {
                min_index = j;
            }
        }
        
        // swap the found min element with the first element
        swap(&array[min_index], &array[i]);
    }
}

/*
 * sort array and compute its average
 * uses Selection Sort algorithm
 * returns sorted array and overall average
 */
void *
sort_thread_avg(void *arg)
{
    ThreadData *data;
    double *array;
    double *avg;
    double sum;
    int length;
    int i;

    data = (ThreadData*)arg;
    array = (double*)malloc(sizeof(double) * data->length);
    length = data->length;
    copy_array(data->array, array, data->length, COPY_ARRAY_INDEX);
    avg = (double*)malloc(sizeof(double));
    
    selection_sort(data->array, data->length);
    
    // NOW FIND THE AVERAGE
    for (i = 0; i < length; ++i) {
        sum += data->array[i];
    }

    *avg = sum / length;
    data->avg = avg;
    pthread_exit(data);
    /* pthread_exit(avg); */
}



