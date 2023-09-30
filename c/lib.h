#ifndef LIB_H
#define LIB_H

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

void print_array(double *array, int length);
int is_numeric(char *str);
double generate_random_double(double min, double max);
void copy_array(double *src, double *dest, int length, int index);

/* Thread Functions */
void *sort_thread_avg(void *arg);
void *merge_thread_avg(void *arg);

#endif
