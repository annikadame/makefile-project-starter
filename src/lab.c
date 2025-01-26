#include "lab.h"
#include <stdio.h>
#include <stdlib.h>

// Initializes a new list
list_t *list_init(void (*destroy_data)(void *), int (*compare_to)(const void *, const void *)) {
    if (!destroy_data || !compare_to) return NULL;

    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (!list) return NULL;

    // Assigns a sentinel node for the list
    node_t *sentinel = (node_t *)malloc(sizeof(node_t));
    if (!sentinel) {
        free(list);
        return NULL;
    }

    sentinel->data = NULL;
    sentinel->next = sentinel;
    sentinel->prev = sentinel;

    list->destroy_data = destroy_data;
    list->compare_to = compare_to;
    list->size = 0;
    list->head = sentinel;

    return list;
}

// Destroys the list and frees all memory
void list_destroy(list_t **list) {
    if (!list || !(*list)) return;

    node_t *current = (*list)->head->next;

    // Free all nodes except the sentinel
    while (current != (*list)->head) {
        node_t *next = current->next;
        if ((*list)->destroy_data) (*list)->destroy_data(current->data);
        free(current);
        current = next;
    }

    // Free the sentinel node and list itself
    free((*list)->head);
    free(*list);
    *list = NULL;
}

// Adds data to the front of the list
list_t *list_add(list_t *list, void *data) {
    if (!list || !data) return NULL;

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) return NULL;

    node_t *head = list->head;

    new_node->data = data;
    new_node->next = head->next;
    new_node->prev = head;

    head->next->prev = new_node;
    head->next = new_node;

    list->size++;
    return list;
}

// Removes the node at the given index and returns its data
void *list_remove_index(list_t *list, size_t index) {
    if (!list || index >= list->size) return NULL;

    node_t *current = list->head->next;

    // Find the node at the specified index
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }

    // Remove the node
    current->prev->next = current->next;
    current->next->prev = current->prev;

    void *data = current->data;
    free(current);
    list->size--;

    return data;
}

// Finds the index of the first occurrence of the requested data
int list_indexof(list_t *list, void *data) {
    if (!list || !data) return -1;

    node_t *current = list->head->next;
    size_t index = 0;

    // Traverse the list and compare data and stops at the sentinel
    while (current != list->head) {
        if (list->compare_to(data, current->data) == 0) {
            return (int)index;
        }
        current = current->next;
        index++;
    }

    return -1;
}
