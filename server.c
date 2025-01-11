#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "safe_socket.h"
#include "server_connection_handler.h"
#include "common_utils.h"
#include "serialization.h"
#include "data_structures/queue.h"

#define THREAD_POOL_SIZE 10

int server_socket;
PhoneDirectory *phone_directory = NULL;
pthread_t threads[THREAD_POOL_SIZE];
volatile sig_atomic_t shutdown_flag = 0;
pthread_mutex_t ready_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t closeable_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_queue_cond = PTHREAD_COND_INITIALIZER;
Queue *ready_queue, *closeable_queue;

void close_closeable_sockets();
void *thread_loop(void *args);
void sigint_handler(int sig_num);

int main() {
    phone_directory = create_phone_directory();
    deserialize(phone_directory);
    /* Signal undefined behaviour when multithreading */
    struct sigaction act = { 0 };
    act.sa_handler = &sigint_handler;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    check(server_socket = create_server_socket(),
        "Error creating server socket");

    ready_queue = (Queue*) malloc(sizeof(Queue));
    closeable_queue = (Queue*) malloc(sizeof(Queue));
    init_queue(ready_queue);
    init_queue(closeable_queue);


    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        check(pthread_create(&threads[i], NULL, thread_loop, NULL),
            "Error creating thread");
    }

    printf("Server online. Waiting for connections...\n");

    while (!shutdown_flag) {

        close_closeable_sockets();

        if (shutdown_flag) {
            break;
        }

        int client_socket = accept_connection(server_socket);
        int *socket_ptr = (int*) malloc(sizeof(int));
        *socket_ptr = client_socket;

        printf("Connection received from socket %d\n", client_socket);
        pthread_mutex_lock(&ready_queue_mutex);
        enqueue(ready_queue, socket_ptr);
        pthread_cond_signal(&ready_queue_cond);
        pthread_mutex_unlock(&ready_queue_mutex);
        printf("Connection %d added to the ready queue\n", client_socket);
    }
    return EXIT_SUCCESS;
}

void *thread_loop(void *args) {
    // Block SIGINT signal for threads
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

    fd_set worker_set, ready_set;
    int set_size = 0;
    FD_ZERO(&worker_set);

    while (!shutdown_flag) {
        int *client_socket = NULL;

        pthread_mutex_lock(&ready_queue_mutex);
        while (!shutdown_flag && (client_socket = dequeue(ready_queue)) == NULL && set_size == 0) {
            pthread_cond_wait(&ready_queue_cond, &ready_queue_mutex);
        }
        pthread_mutex_unlock(&ready_queue_mutex);

        if (shutdown_flag) {
            break;
        }
        if (client_socket != NULL) {
            FD_SET(*client_socket, &worker_set);
            set_size++;
        }

        ready_set = worker_set;
        check(select(FD_SETSIZE, &ready_set, NULL, NULL, NULL), "Select failed");

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_set)) {
                int *socket = (int*) malloc(sizeof(int));
                *socket = i;
                printf("Handling connection %d in thread %lu\n", i, pthread_self());
                if (handle_connection(*socket, phone_directory) <= 0) {
                    pthread_mutex_lock(&closeable_queue_mutex);
                    enqueue(closeable_queue, socket);
                    pthread_mutex_unlock(&closeable_queue_mutex);
                    FD_CLR(*socket, &worker_set);
                    set_size--;
                    printf("Thread %lu finished handling connection %d\n", pthread_self(), *socket);
                }
            }
        }
    }
    return NULL;
}

void close_closeable_sockets() {
    pthread_mutex_lock(&closeable_queue_mutex);
    int *current;
    while ((current = dequeue(closeable_queue)) != NULL) {
        close(*current);
        free(current);
    }
    pthread_mutex_unlock(&closeable_queue_mutex);
}

void sigint_handler (int sig_num){
    printf("Shutting down server...\n\n");
    shutdown_flag = 1;
    close(server_socket);
    pthread_cond_broadcast(&ready_queue_cond);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Saving phone directory...\n");
    while (!serialize(phone_directory)) {
        printf("Error saving phone directory. Retrying...\n");
        sleep(2);
    }
    printf("Phone directory saved\n\n");

    printf("Closing server socket...\n");
    close_closeable_sockets();
    printf("Server socket closed\n\n");

    printf("Freeing resources...\n");

    free_queue(ready_queue);
    printf("Ready queue freed\n");

    free_queue(closeable_queue);
    printf("Closeable queue freed\n");

    free_phone_directory(phone_directory);
    printf("Phone directory freed\n\n");

    printf("Server shutdown\n");
}