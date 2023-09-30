
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
void *merge_thread_avg(void *arg0, void *arg1);


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
    double *A_first_avg;
    double *A_second_avg;
    double *overall_avg;
    char *endptr;
    char *str;
    long n;
    long n_half;
    size_t A_byte_size;
    struct timespec ts_begin;
    struct timespec ts_end;

    // thread IDs
    pthread_t tid_1;
    pthread_t tid_2;
    pthread_t tid_3;

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
    
    // Assign n_half = n / 2
    n_half = n / 2;

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
    copy_array(A, B, n, COPY_ARRAY_INDEX);
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);
    ThreadData *B_data = (ThreadData*)malloc(sizeof(ThreadData));
    B_data->array = B;
    B_data->length = n;
    
    pthread_create(&tid_1, NULL, sort_thread_avg, (void *)B_data);
    pthread_join(tid_1, (void **)&B_data);

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsed = ts_end.tv_sec - ts_begin.tv_sec;
    elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;
    
    /* Sorting is done in 10.0ms when one thread is used */
    fprintf(stdout, "Sorting is done in %f when one thread is used\n",
            elapsed);
    if (n < 10) {
        fprintf(stdout, "Average of sorted array: %10f ms\n", *B_data->avg);
        print_array(B_data->array, n);
    } else {
        fprintf(stdout, "Average of sorted array: %f\n", *B_data->avg);
        print_array(B_data->array, 10);
    }
    
    free(B_data->avg);
    free(B_data);


    /*********** TWO THREADS CASE *********************/
    /* copy A into A_first_half and A_second_half */
    copy_array(A, A_first_half, n_half, COPY_ARRAY_INDEX);
    copy_array(A, A_second_half, n_half, COPY_ARRAY_INDEX);
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);
    // create threadA1 sortThread_avg to sort AFirsthalf and compute Afirstavg
    // Do the same for the second thread
    ThreadData *A_first_half_data = (ThreadData*)malloc(sizeof(ThreadData));
    ThreadData *A_second_half_data = (ThreadData*)malloc(sizeof(ThreadData));
    A_first_half_data->array = A_first_half;
    A_first_half_data->length = n_half;
    A_second_half_data->array = A_second_half;
    A_second_half_data->length = n_half;
    
    pthread_create(&tid_2, NULL, sort_thread_avg, (void *)A_first_half_data);
    pthread_create(&tid_3, NULL, sort_thread_avg, (void *)A_second_half_data);
    pthread_join(tid_2, (void **)&A_first_half_data);
    pthread_join(tid_3, (void **)&A_second_half_data);
    
    // create a thM megaThread_avg by passing A_first_half, A_second_half, A_first_avg, A_second_avg
    // make sure this just merges the sorted values from two arrays while keeping them sorted O(n)
    // don't copy these array sand then call sort! which will be like sorting the whole thing!
    // it's supposed to simply compute overall_avg = (A_first_avg + a_first_avg) / 2
    // return merged/sorted array and overall_avg
    
    
    // join thM
    

    /* Sorting by TWO threads is done in elasped * 1000 ms */
    /* Print overall_avg and at least first 10 values of the sorted array */
    




    /*********** FREE MEMORY *****************/
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
    
    /* one by one move the boundary of the unsorted array */
    for (i = 0; i < n - 1; ++i) {
        /* find the minimum element in the unsorted array */
        min_index = i;
        for (j = i + 1; j < n; ++j) {
            if (array[j] < array[min_index]) {
                min_index = j;
            }
        }
        
        /* swap the found min element with the first element */
        swap(&array[min_index], &array[i]);
    }
}


static double *
merge_sorted_arrays(double *A, double *B, int A_length, int B_length)
{
    int n = A_length;
    int m = B_length;
    int i;
    int j;
    int k;
    double *C;
    
    i = 0;
    j = 0;
    k = 0;
    C = (double*)malloc(sizeof(A) + sizeof(B));

    if (C == NULL || !C) {
        fprintf(stderr, 
                "Memory was not successfully allocated into C array in merge_sorted_arrays\n");
        free(C);
        exit(EXIT_FAILURE);
    }

    while (i < n && j < m) {
        if (A[i] <= B[j]) {
            C[k] = A[i];
            ++i;
        } else {
            C[k] = B[j];
            ++j;
        }
        ++k;
    }

    /* if there are remaining elements in A */
    while (i < n) {
        C[i] = A[i];
        ++i;
        ++k;
    }

    /* if there are remaining elements in B */
    while (j < n) {
        C[k] = B[j];
        ++j;
        ++k;
    }
    
    return C;
}


void *
sort_thread_avg(void *arg)
{
    ThreadData *data;
    double *array;
    double *avg;
    double sum = 0.0;
    int length;
    int i;

    data = (ThreadData*)arg;
    array = (double*)malloc(sizeof(double) * data->length);
    
    /* checks for successful memory allocation */
    if(!array) {   
        pthread_exit(NULL);
    }
    
    length = data->length;
    copy_array(data->array, array, data->length, COPY_ARRAY_INDEX);
    avg = (double*)malloc(sizeof(double));

    /* checks for successful memory allocation */
    if(!avg) { 
        free(array);  /* free previously allocated memory */
        pthread_exit(NULL);
    }
    
    selection_sort(data->array, data->length);
    
    /* Find the average */
    for (i = 0; i < length; ++i) {
        sum += data->array[i];
    }
    *avg = sum / length;
    data->avg = avg;
    
    free(array);  /* Free memory allocated for array */

    pthread_exit(data);
}



void *
merge_thread_avg(void *arg0, void *arg1)
{
    ThreadData *data_0;
    ThreadData *data_1;
    ThreadData *merged_data;
    
    double *data_0_array;
    double *data_1_array;
    double *merged_data_array;

    double *data_0_avg;
    double *data_1_avg;
    double *merged_data_avg;

    double data_0_sum = 0.0;
    double data_1_sum = 0.0;
    double merged_data_sum = 0.0;

    int data_0_length = 0;
    int data_1_length = 0;
    int merged_data_lengh = 0;

    int i = 0;

    /* assign the args */
    data_0 = (ThreadData*)arg0;
    data_1 = (ThreadData*)arg1;
    data_0_array = (double*)malloc(sizeof(double) * data_0->length);
    data_1_array = (double*)malloc(sizeof(double) * data_1->length);
    
    /* check for successful memory allocation */
    if (!data_0_array || !data_1_array) {
        fprintf(stderr, "Memory was not successfully allocated to arrays ih merge_thread_avg\n");
        free(data_0_array);
        free(data_1_array);
        pthread_exit(NULL);
    }

    /* assign length */
    data_0_length = data_0->length;
    data_1_length = data_1->length;
    
    /* copy arrays */
    copy_array(data_0->array, data_0_array, data_0_length, COPY_ARRAY_INDEX);
    copy_array(data_1->array, data_1_array, data_1_length, COPY_ARRAY_INDEX);
    
    /* allocate memory for averages */
    data_0_avg = (double*)malloc(sizeof(double));
    data_1_avg = (double*)malloc(sizeof(double));

    /* check for successful memory allocation of averages */
    if (!data_0_avg || !data_1_avg) {
        fprintf(stdout, "Memory was not successfully allocated to averages in merge_thread_avg\n");
        free(data_0_avg);
        free(data_1_avg);
        pthread_exit(NULL);
    }

    *data_0_avg = data_0->avg;
    *data_1_avg = data_1->avg;

    return NULL; // keep this here for now    
}



    /* function mergeSortedArrays(A, B): */
    /* n = length(A) */
    /* m = length(B) */
    /* C = new array of size (n + m) */
    
    /* i = 0   // pointer for array A */
    /* j = 0   // pointer for array B */
    /* k = 0   // pointer for merged array C */

    /* while i < n and j < m: */
    /*     if A[i] <= B[j]: */
    /*         C[k] = A[i] */
    /*         i = i + 1 */
    /*     else: */
    /*         C[k] = B[j] */
    /*         j = j + 1 */
    /*     k = k + 1 */

    /* // If there are remaining elements in A */
    /* while i < n: */
    /*     C[k] = A[i] */
    /*     i = i + 1 */
    /*     k = k + 1 */

    /* // If there are remaining elements in B */
    /* while j < m: */
    /*     C[k] = B[j] */
    /*     j = j + 1 */
    /*     k = k + 1 */

    /* return C */

    
    



