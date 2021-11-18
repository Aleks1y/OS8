#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define NUMBER_OF_ARGUMENTS 3
#define MAXIMUM_NUMBER_OF_THREADS 10000
#define MINIMUM_NUMBER_OF_THREADS 1
#define MAXIMUM_NUMBER_OF_STEPS 1000000000
#define SUCCESS 0
#define ERROR 1

typedef struct thread_parameters {
    int thread_number;
    int number_of_threads;
    int number_of_steps;
    double part_pi;
} thread_parameters;

void* thread_body(void* thread_parameter) {
    thread_parameters* current_parameter = (thread_parameters*)thread_parameter;
    current_parameter->part_pi = 0;
	
    for (int i = current_parameter->thread_number; i < current_parameter->number_of_steps; i += current_parameter->number_of_threads) {
        current_parameter->part_pi += 1.0 / (i * 4.0 + 1.0);
        current_parameter->part_pi -= 1.0 / (i * 4.0 + 3.0);
    }

    return &current_parameter->part_pi;
}

int checkInput(int argc, char** argv) {
    if (argc < NUMBER_OF_ARGUMENTS) {
        fprintf(stderr, "Not enough arguments\n");
        return ERROR;
    }
	
    for(int i = 0; i < strlen(argv[1]); i++)
    	if (!isdigit(argv[1][i])) {
            fprintf(stderr, "Wrong number of threads\n");
            return ERROR;
    	}
	
    for(int i = 0; i < strlen(argv[2]); i++)
    	if (!isdigit(argv[2][i])) {
            fprintf(stderr, "Wrong number of steps\n");
            return ERROR;
    	}
	
    if (atoi(argv[1]) > MAXIMUM_NUMBER_OF_THREADS) {
        fprintf(stderr, "Too many threads\n");
        return ERROR;
    }

    if (atoi(argv[1]) < MINIMUM_NUMBER_OF_THREADS) {
        fprintf(stderr, "Too few threads\n");
        return ERROR;
    }

    if (atoi(argv[2]) > MAXIMUM_NUMBER_OF_STEPS) {
        fprintf(stderr, "Too many step numbers\n");
        return ERROR;
    }

    return SUCCESS;
}

int calc_pi(int number_of_threads, int number_of_steps, double* pi) {
    pthread_t threads[number_of_threads];
    thread_parameters parameters[number_of_threads];

    for (int i = 0; i < number_of_threads; ++i) {
        parameters[i].thread_number = i;
        parameters[i].number_of_threads = number_of_threads;
        parameters[i].number_of_steps = number_of_steps;
        errno = pthread_create(&threads[i], NULL, thread_body, (void*)(&parameters[i]));
        if (errno != SUCCESS) {
            perror("Creating thread error");
            return ERROR;
        }
    }
	
    double* part_pi = 0;
    for (int i = 0; i < number_of_threads; ++i) {
        errno = pthread_join(threads[i], (void**)(&part_pi));
        if (errno != SUCCESS) {
            perror("Joining thread error");
            return ERROR;
        }
        *pi += *part_pi;
    }

    *pi *= 4.0;
	
    return SUCCESS;
}

int main(int argc, char** argv) {
    if (checkInput(argc, argv) != SUCCESS) {
        exit(EXIT_FAILURE); 
    }
	
    int number_of_threads = atoi(argv[1]);
    int number_of_steps = atoi(argv[2]);
    double pi = 0;

    if (calc_pi(number_of_threads, number_of_steps, &pi) != SUCCESS) {
        exit(EXIT_FAILURE); 
    }
	
    printf("pi done - %.15f \n", pi);
    exit(0);
}
