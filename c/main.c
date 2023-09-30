

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

typedef struct MergedThreadData {
    struct ThreadData *th_data_0;
    struct ThreadData *th_data_1;
    struct ThreadData *th_merged_data;
} MergedThreadData;


/****************** HELPER FUNCTIONS ******************************/
/* Basic Help Functions */
void print_array(double *array, int length);
int is_numeric(char *str);
double generate_random_double(double min, double max);
void copy_array(double *src, double *dest, int length, int index);

/* Thread Functions */
void *sort_thread_avg(void *arg);
void *merge_thread_avg(void *arg);


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
    pthread_t tid_4;

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
    n_half = (n + 1) / 2;

    // Allocate the arrays
    A_byte_size = sizeof(double) * n;
    A = (double*)malloc(A_byte_size);
    B = (double*)malloc(A_byte_size);
    C = (double*)malloc(A_byte_size);
    A_first_half = (double*)malloc(n_half * sizeof(double));
    A_second_half = (double*)malloc(n_half * sizeof(double));

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
            elapsed * 1000);
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
    /* size_t num_elements = 100; */
    /* int* originalArray = malloc(num_elements * sizeof(int)); */
    /* size_t half_elements = (num_elements + 1) / 2;  // this will round up if odd */

    /* // Populate the original array as an example */
    /* for (size_t i = 0; i < num_elements; i++) { */
    /*     originalArray[i] = i; */
    /* } */

    /* // Allocate memory for the second half */
    /* int* secondHalfArray = malloc(half_elements * sizeof(int)); */

    /* // Copy the second half of originalArray to secondHalfArray */
    /* memcpy(secondHalfArray, &originalArray[num_elements / 2], half_elements * sizeof(int)); */
    
    /* memcpy(A_first_half, &A[n / 2], n_half * sizeof(double)); */
    /* memcpy(A_second_half, &A[n - (n / 2)], n_half * sizeof(double)); */
    
        printf("\nSTARTING TWO THREADS CASE\n\n");
    printf("\nCopying First half array\n");
    for (i = 0; i < n_half; ++i) {
        printf("A[i]: %f\n", A[i]);
        A_first_half[i] = A[i];
    }
    int j = 0;
    printf("\nCopying Second half array\n\n");
    for (i = n_half; i < n; ++i) {
        printf("A[i]: %f\n", A[i]);
        A_second_half[j] = A[i];
        j++;
    }
 



    clock_gettime(CLOCK_MONOTONIC, &ts_begin);
    // create threadA1 sortThread_avg to sort AFirsthalf and compute Afirstavg
    // Do the same for the second thread
    ThreadData *A_first_half_data = (ThreadData*)malloc(sizeof(ThreadData));
    ThreadData *A_second_half_data = (ThreadData*)malloc(sizeof(ThreadData));
    
    
    A_first_half_data->array = A_first_half;
    printf("A first half data array was assigned\n");
    print_array(A_first_half_data->array, n_half);
    A_first_half_data->length = n_half;


    A_second_half_data->array = A_second_half;
    printf("A second half datay array was assigned\n");
    print_array(A_second_half_data->array, n_half);
    A_second_half_data->length = n_half;
    
    printf("\nSTARTING 2 PTHREAD_CREATE\n\n");
    pthread_create(&tid_2, NULL, sort_thread_avg, (void *)A_first_half_data);
    pthread_create(&tid_3, NULL, sort_thread_avg, (void *)A_second_half_data);
    printf("\nSTARTING 2 PTHREAD JOIN\n\n");
    pthread_join(tid_2, (void **)&A_first_half_data);
    pthread_join(tid_3, (void **)&A_second_half_data);

    // create a thM megaThread_avg by passing A_first_half, A_second_half, A_first_avg, A_second_avg
    MergedThreadData *merged_data = (MergedThreadData*)malloc(sizeof(MergedThreadData));
    merged_data->th_data_0 = malloc(sizeof(ThreadData));
    merged_data->th_data_1 = malloc(sizeof(ThreadData));
    merged_data->th_merged_data = malloc(sizeof(ThreadData));

    merged_data->th_data_0->array = A_first_half_data->array;
    merged_data->th_data_1->array = A_second_half_data->array;
    merged_data->th_data_0->avg = A_first_half_data->avg;
    merged_data->th_data_1->avg = A_second_half_data->avg;
    merged_data->th_data_0->length = A_first_half_data->length;
    merged_data->th_data_1->length = A_second_half_data->length; 


    /* ThreadData *merged_data = (ThreadData*)malloc(sizeof(ThreadData)); */
    /* MergedThreadData *merged_data = (MergedThreadData*)malloc(sizeof(MergedThreadData)); */
    /* merged_data->th_data_0->array = A_first_half_data->array; */
    /* merged_data->th_data_1->array = A_second_half_data->array; */
    /* merged_data->th_data_0->avg = A_first_half_data->avg; */
    /* merged_data->th_data_1->avg = A_second_half_data->avg; */
    /* merged_data->th_data_0->length = A_first_half_data->length; */
    /* merged_data->th_data_1->length = A_second_half_data->length; */
    /* merged_data->th_merged_data = (ThreadData*)malloc(sizeof(ThreadData)); */
    
    printf("\nSTARTING MERGE THREAD AVGERAGE CREATE\n\n");
    pthread_create(&tid_4, NULL, merge_thread_avg, (void *)merged_data);
    printf("\nSTARTING MERGE THREAD AVERAGE JOIN\n\n");
    pthread_join(tid_4, (void **)&merged_data);
    printf("\nFNISHED MERGED DATA PTHREAD JOIN\n\n");

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsed = ts_end.tv_sec - ts_begin.tv_sec;
    elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /  1000000000.0;
    
    printf("\nSTARTING TO PRINT OUT RESULTS\n\n");

    

    




    /*********** FREE MEMORY *****************/
    free(A_first_half_data->avg);
    free(A_second_half_data->avg);
    free(A_first_half_data);
    free(A_second_half_data);
    free(merged_data->th_data_0);
    free(merged_data->th_data_1);
    free(merged_data->th_merged_data);
    free(merged_data);
    free(A);
    free(B);
    free(C);
    free(A_first_half);
    free(A_second_half);

    /* free(A); */
    /* free(B); */
    /* free(C); */
    /* free(A_first_half); */
    /* free(A_second_half); */

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

void*
merge_thread_avg(void *arg)
{
    MergedThreadData *merged_thread_data;
    
    double *array;
    double *avg;
    double sum = 0.0;
    int length;
    int i;
    
    

    merged_thread_data = (MergedThreadData*)arg;
    length = (merged_thread_data->th_data_0->length + 
            merged_thread_data->th_data_1->length);
    array = (double*)malloc(sizeof(double) * length);
    
    if (!array) {
        fprintf(stderr, "Unsuccessfull malloc to array in merge_thread_avg\n");
        free(array);
        pthread_exit(NULL);
    }
  
    merged_thread_data->th_merged_data->array = 
        (double*)malloc(sizeof(double) * length);

    memcpy(
            array,
            merged_thread_data->th_data_0->array,
            merged_thread_data->th_data_0->length * sizeof(double));
    memcpy(
            array + merged_thread_data->th_data_0->length,
            merged_thread_data->th_data_1->array,
            merged_thread_data->th_data_1->length * sizeof(double));

    for (i = 0; i < length; ++i) {
        printf("merged_array[i]: %f\n", array[i]);
    }

    avg = (double*)malloc(sizeof(double));
    if (!avg) {
        fprintf(stderr, "Unsuccessfull malloc to avg in merge_thread_avg\n");
        pthread_exit(NULL);
    }
    
    selection_sort(array, length);
    
    printf("\nMEMCPY\n\n");
    memcpy(
            merged_thread_data->th_merged_data->array,
            array,
            length * sizeof(double));
    printf("\nEND OF MEMCPY\n\n");
    *avg = (*merged_thread_data->th_data_0->avg + *merged_thread_data->th_data_1->avg) / 2;
    merged_thread_data->th_merged_data->avg = avg;
    merged_thread_data->th_merged_data->length = length;

    free(array);
    
    pthread_exit(merged_thread_data);
}






