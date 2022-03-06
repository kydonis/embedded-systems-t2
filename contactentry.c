#include <stdlib.h>
#include "contactentry.h"

struct contactentry *createContactEntry(struct macaddress mac, uint64_t lastSeen) {
    struct contactentry *entry = (struct contactentry *) malloc(sizeof(struct contactentry));
    entry->mac = mac;
    entry->lastSeen = lastSeen;
    return entry;
}
