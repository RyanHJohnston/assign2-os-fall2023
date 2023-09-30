#include "lib.h"

int 
main(int argc, char *argv[]) 
{
    /* general variables */
    int i;
    int base;
    double *avg;
    double *A_first_avg;
    double *A_second_avg;
    double *overall_avg;
    double elapsed;
    char *endptr;
    char *str;
    long n;
    long n_half;
    size_t A_byte_size;
    struct timespec ts_begin;
    struct timespec ts_end;
    
    /* threads IDs */
    pthread_t tid_1;
    pthread_t tid_2;
    pthread_t tid_3;
    pthread_t tid_4;
    
    /* dynamic */
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

    n_half = (n + 1) / 2;

    /* allocate the arrays */
    A_byte_size = sizeof(double) * n;
    A = (double*)malloc(A_byte_size);
    B = (double*)malloc(A_byte_size);
    C = (double*)malloc(A_byte_size);
    A_first_half = (double*)malloc(n_half * sizeof(double));
    A_second_half = (double*)malloc(n_half * sizeof(double));
    
    /* init random seed, ensure random values on different runs */
    srand((unsigned int)time(NULL)); 

    /* generates an array of random values */
    for (i = 0; i < n; ++i) {
        A[i] = generate_random_double(DOUBLE_MIN, DOUBLE_MAX);
    }

    /* thread one case */
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
    fprintf(stdout, "Sorting is done in %f ms when one thread is used\n",
            elapsed * 1000);
    /* if (n < 10) { */
    /*     fprintf(stdout, "Average of sorted array: %10f ms\n", *B_data->avg); */
    /*     print_array(B_data->array, n); */
    /* } else { */
    /*     fprintf(stdout, "Average of sorted array: %f\n", *B_data->avg); */
    /*     print_array(B_data->array, 10); */
    /* } */
    
    free(B_data->avg);
    free(B_data);

    /* threads two case */
    for (i = 0; i < n_half; ++i) {
        A_first_half[i] = A[i];
    }

    int j = 0;
    for (i = n_half; i < n; ++i) {
        A_second_half[j] = A[i];
        j++;
    }
 
    clock_gettime(CLOCK_MONOTONIC, &ts_begin);

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

    pthread_create(&tid_4, NULL, merge_thread_avg, (void *)merged_data);
    pthread_join(tid_4, (void **)&merged_data);

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsed = ts_end.tv_sec - ts_begin.tv_sec;
    elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) /  1000000000.0;
    
    fprintf(stdout, "Sorting is done in %f ms when two threads are used\n",
            elapsed * 1000);
    /* if (merged_data->th_merged_data->length < 10) { */
    /*     fprintf(stdout, "Average of merged sorted array: %f\n", */
    /*             *merged_data->th_merged_data->avg); */
    /*     print_array(merged_data->th_merged_data->array, n); */
    /* } else { */
    /*     fprintf(stdout, "Average of merged sorted array: %f\n", */
    /*             *merged_data->th_merged_data->avg); */
    /*     print_array(merged_data->th_merged_data->array, 10); */
    /* } */

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

    return 0;
}
