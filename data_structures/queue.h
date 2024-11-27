#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node {
    struct Node *next;
    int *client_socket;
} Node;

typedef struct Queue {
  Node *head;
  Node *tail;
} Queue;

void init_queue(Queue *queue);
void enqueue(Queue *queue, int *client_socket);
int *dequeue(Queue *queue);
void free_queue(Queue *queue);

#endif
