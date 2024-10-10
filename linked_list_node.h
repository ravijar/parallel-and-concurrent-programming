#ifndef LINKED_LIST_COMMON_H
#define LINKED_LIST_COMMON_H

// Linked list node structure definition
typedef struct list_node_t {
    int data;
    struct list_node_t* next;
} list_node_t;

#endif  // LINKED_LIST_COMMON_H
