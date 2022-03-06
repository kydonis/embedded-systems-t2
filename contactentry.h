#include "macaddress.h"

struct contactentry {
    struct macaddress mac;
    uint64_t lastSeen;
};

struct contactentry *createContactEntry(struct macaddress mac, uint64_t lastSeen);
