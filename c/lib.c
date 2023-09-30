#include "lib.h"

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
    
    avg = (double*)malloc(sizeof(double));
    if (!avg) {
        fprintf(stderr, "Unsuccessfull malloc to avg in merge_thread_avg\n");
        pthread_exit(NULL);
    }
    
    selection_sort(array, length);
    
    memcpy(
            merged_thread_data->th_merged_data->array,
            array,
            length * sizeof(double));
    *avg = (*merged_thread_data->th_data_0->avg + *merged_thread_data->th_data_1->avg) / 2;
    merged_thread_data->th_merged_data->avg = avg;
    merged_thread_data->th_merged_data->length = length;

    free(array);
    
    pthread_exit(merged_thread_data);
}

