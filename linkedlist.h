#include <stdint.h>
#include <stdlib.h>
#include "contactentry.h"

struct LinkedList {
    uint64_t size;
    struct Node *head;
    struct Node *tail;
};

struct Node {
    struct Node *previous;
    struct contactentry *data;
    struct Node *next;
};

struct LinkedList *createLinkedList();

int isEmpty(struct LinkedList *linkedList);

void push(struct LinkedList *linkedList, struct contactentry *item);

void removeNode(struct LinkedList *linkedList, struct Node *node);

struct macaddress *getMacAddressesArray(struct LinkedList *linkedList);

int containsMacAddress(struct LinkedList *linkedList, struct macaddress macaddress);
