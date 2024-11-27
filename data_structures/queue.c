#include "queue.h"
#include <stddef.h>
#include <stdlib.h>

void init_queue(Queue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

void enqueue(Queue *queue, int *client_socket) {

    Node *new_node = (Node*) malloc(sizeof(Node));
    new_node->client_socket = client_socket;
    new_node->next = NULL;

    if (queue->tail == NULL) {
        queue->head = new_node;
    } else {
        queue->tail->next = new_node;
    }
    queue->tail = new_node;
}

/*Return NULL if there are no elements in the queue*/
int *dequeue(Queue *queue) {
    if (queue->head == NULL) {
        return NULL;
    } else {
        int* result = queue->head->client_socket;
        Node *temp = queue->head;
        queue->head = queue->head->next;
        if (queue->head == NULL) {
            queue->tail = NULL;
        }
        return result;
    }
}

void free_queue(Queue *queue) {
    Node *current = queue->head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp->client_socket);
        free(temp);
    }
}
