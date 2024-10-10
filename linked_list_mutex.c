#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct list_node_s {
    int data;
    struct list_node_s* next;
} list_node_t;

pthread_mutex_t mutex;

int Member(int value, list_node_t* head_p) {
    pthread_mutex_lock(&mutex);
    list_node_t* curr_p = head_p;
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;
    if (curr_p == NULL || curr_p->data > value) {
        pthread_mutex_unlock(&mutex);
        return 0;
    } else {
        pthread_mutex_unlock(&mutex);
        return 1;
    }
}

int Insert(int value, list_node_t** head_pp) {
    pthread_mutex_lock(&mutex);
    list_node_t* curr_p = *head_pp;
    list_node_t* pred_p = NULL;
    list_node_t* temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(list_node_t));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;
        pthread_mutex_unlock(&mutex);
        return 1;
    } else {
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

int Delete(int value, list_node_t** head_pp) {
    pthread_mutex_lock(&mutex);
    list_node_t* curr_p = *head_pp;
    list_node_t* pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL)
            *head_pp = curr_p->next;
        else
            pred_p->next = curr_p->next;
        free(curr_p);
        pthread_mutex_unlock(&mutex);
        return 1;
    } else {
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}
