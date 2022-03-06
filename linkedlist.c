#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

struct LinkedList *createLinkedList() {
    struct LinkedList *linkedList = (struct LinkedList *) malloc(sizeof(struct LinkedList));
    linkedList->size = 0;
    linkedList->head = NULL;
    linkedList->tail = NULL;
    return linkedList;
}

int isEmpty(struct LinkedList *linkedList) {
    return linkedList->size == 0;
}

void push(struct LinkedList *linkedList, struct contactentry *item) {
    linkedList->size += 1;
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node->data = item;
    node->next = NULL;
    node->previous = NULL;
    if (linkedList->head == NULL) {
        linkedList->head = node;
        linkedList->tail = node;
    } else {
        linkedList->tail->next = node;
        node->previous = linkedList->tail;
        linkedList->tail = node;
    }
}

void removeNode(struct LinkedList *linkedList, struct Node *node) {
    linkedList->size -= 1;
    if (node->previous != NULL) {
        if (node->next != NULL) {
            node->previous->next = node->next;
            node->next->previous = node->previous;
        } else {
            node->previous->next = NULL;
            linkedList->tail = node->previous;
        }
    } else {
        linkedList->head = linkedList->head->next;
        if (linkedList->head == NULL) {
            linkedList->tail = NULL;
        }
    }
}

struct macaddress *getMacAddressesArray(struct LinkedList *linkedList) {
    struct macaddress *macAddressArray = (struct macaddress *) malloc(linkedList->size * sizeof(struct macaddress));
    uint64_t i = 0;
    for (struct Node *currentNode = linkedList->head; currentNode != NULL; currentNode = currentNode->next) {
        struct contactentry *current = currentNode->data;
        macAddressArray[i] = current->mac;
        i++;
    }
    return macAddressArray;
}

int containsMacAddress(struct LinkedList *linkedList, struct macaddress macaddress) {
    for (struct Node *currentNode = linkedList->head; currentNode != NULL; currentNode = currentNode->next) {
        struct contactentry *current = currentNode->data;
        if (current->mac.mac == macaddress.mac) {
            return 1;
        }
    }
    return 0;
}
