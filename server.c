#include "safe_socket.h"
#include <sys/select.h>
#include <pthread.h>
#include "data_structures/queue.h"
#include <signal.h>

#define THREAD_POOL_SIZE 10

pthread_t threads[THREAD_POOL_SIZE];
volatile sig_atomic_t shutdown_flag = 0;
pthread_mutex_t ready_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t closeable_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_queue_cond = PTHREAD_COND_INITIALIZER;


Queue *ready_queue, *closeable_queue;

int handle_connection(int socket);
void close_closeable_sockets();
void *thread_loop(void *args);
void sigint_handler(int signum);

int main() {
    int server_socket;
    signal(SIGINT, sigint_handler);
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

        //TODO: Find a way to avoid blocking accept with SIGINT
        int client_socket = accept_connection(server_socket);
        int *socket_ptr = (int*) malloc(sizeof(int));
        *socket_ptr = client_socket;

        printf("Connection received from %d\n", client_socket);
        pthread_mutex_lock(&ready_queue_mutex);
        enqueue(ready_queue, socket_ptr);
        pthread_cond_signal(&ready_queue_cond);
        pthread_mutex_unlock(&ready_queue_mutex);
        printf("Connection %d added to ready queue\n", client_socket);
    }

    close(server_socket);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    close_closeable_sockets();
    printf("Freeing resources...\n");
    free_queue(ready_queue);
    printf("Ready queue freed\n");
    free_queue(closeable_queue);
    printf("Closeable queue freed\n");

    return EXIT_SUCCESS;
}

void *thread_loop(void *args) {

    fd_set worker_set, ready_set;
    int set_size = 0;
    FD_ZERO(&worker_set);

    while (!shutdown_flag) {

        int *client_socket;

        pthread_mutex_lock(&ready_queue_mutex);
        while ((client_socket = dequeue(ready_queue)) == NULL && set_size == 0) {
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
                printf("Handling connection %d in thread %p\n", i, pthread_self());
                if (handle_connection(*socket) <= 0) {
                    pthread_mutex_lock(&closeable_queue_mutex);
                    enqueue(closeable_queue, socket);
                    pthread_mutex_unlock(&closeable_queue_mutex);
                    FD_CLR(*socket, &worker_set);
                    set_size--;
                    printf("Thread %p finished handling connection %d\n", pthread_self(), *socket);
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

int handle_connection(int socket) {
    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        return -1;
    }

    ssize_t msg_size = read_message(socket, buffer);

    if (msg_size < 0) {
        printf("Error reading from client\n");
        free(buffer);
        return -1;
    }

    if (msg_size == 0) {
        printf("Client disconnected\n");
        free(buffer);
        return 0;
    }

    printf("Received: %s \n", buffer);

    if (write_message(socket, buffer) < 0) {
        printf("Error writing to client\n");
        free(buffer);
        return -1;
    }

    free(buffer);
    return 1;
}

void sigint_handler(int signum) {
    printf("Shutting down server...\n");
    shutdown_flag = 1;
    pthread_cond_broadcast(&ready_queue_cond);
    printf("Server shutdown\n");
}
