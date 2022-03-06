#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "linkedlist.h"
#include "covidTrace.h"

// Parameters
uint64_t TIME_FACTOR = 100;
double SECONDS_PER_MOMENT = 10.0;
uint64_t ADDRESS_COUNT = 100000;
uint16_t COVID_TEST_POSITIVITY_PERCENTAGE = 10;

uint64_t *addresses;

struct macaddress BTnearMe() {
    struct macaddress *mac = (struct macaddress *) malloc(sizeof(struct macaddress));
    mac->mac = addresses[rand() % ADDRESS_COUNT];
    return *mac;
}

bool testCOVID() {
    uint16_t r = rand() % 100;
    return r < COVID_TEST_POSITIVITY_PERCENTAGE;
}

void uploadContacts(struct macaddress* macAddresses, uint64_t length) {
    FILE *file = fopen("uploadedAddresses.bin", "ab");
    for (uint64_t i = 0; i < length; i++) {
        fprintf(file, "%012llx\n", macAddresses[i].mac);
    }
    fprintf(file, "\n");
    fclose(file);
}

void readAddresses(uint64_t *addresses) {
    FILE *file = fopen("addresses.txt", "r");
    uint64_t i = 0;
    uint64_t num;
    while (fscanf(file, "%llx", &num) > 0 && i < ADDRESS_COUNT) {
        addresses[i] = num;
        i++;
    }
    fclose(file);
}

void printMacAddress(uint64_t address) {
    printf("[DEBUG] macaddress = %02llx:%02llx:%02llx:%02llx:%02llx:%02llx (%012llx)\n",
           (address << 16) >> 56,
           (address << 24) >> 56,
           (address << 32) >> 56,
           (address << 40) >> 56,
           (address << 48) >> 56,
           (address << 56) >> 56,
           address);
}

double getCurrentTimeInSeconds() {
    struct timespec ts_current;
    clock_gettime(CLOCK_MONOTONIC, &ts_current);
    return ts_current.tv_sec + ts_current.tv_nsec / 1000000000.0;
}

void writeContactsToFile(struct LinkedList *contacts, bool testResult, uint64_t moment) {
    FILE *file = fopen("testResults.bin", "ab");
    fprintf(file, "Covid test = %d at moment %llu\n", testResult, moment);
    for (struct Node *currentNode = contacts->head; currentNode != NULL; currentNode = currentNode->next) {
        fprintf(file, "%012llx\n", currentNode->data->mac.mac);
    }
    fprintf(file, "\n");
    fclose(file);
}

int main() {
    srand(time(NULL));
    addresses = (uint64_t *) malloc(ADDRESS_COUNT * sizeof(uint64_t));
    readAddresses(addresses);
    struct LinkedList *recentAddresses = createLinkedList();
    struct LinkedList *closeContacts = createLinkedList();

    FILE *timeFile = fopen("times.bin", "wb");
    FILE *delaysFile = fopen("delays.bin", "wb");
    double startTime = getCurrentTimeInSeconds();
    double nextCycleTargetTime = startTime + SECONDS_PER_MOMENT / TIME_FACTOR;
    uint64_t moment = 0;
    while (1) {
        struct macaddress r = BTnearMe();
        double calledAt = (getCurrentTimeInSeconds() - startTime) * TIME_FACTOR;
        fprintf(timeFile, "%lf\n", calledAt);
//        printMacAddress(r.mac);

        int closeContactsUpdated = 0;

        for (struct Node *currentNode = closeContacts->head; currentNode != NULL; currentNode = currentNode->next) {
            struct contactentry *current = currentNode->data;
            if (moment - current->lastSeen > 14 * 24 * 60 * 60 / SECONDS_PER_MOMENT) {
                removeNode(closeContacts, currentNode);
                closeContactsUpdated = 1;
            } else if (current->mac.mac == r.mac) {
                current->lastSeen = moment;
            }
        }

        int found = 0;
        for (struct Node *currentNode = recentAddresses->head; currentNode != NULL; currentNode = currentNode->next) {
            struct contactentry *current = currentNode->data;
            if (moment - current->lastSeen > 20 * 60 / SECONDS_PER_MOMENT) {
                removeNode(recentAddresses, currentNode);
            } else if (current->mac.mac == r.mac) {
                if (moment - current->lastSeen >= 4 * 60 / SECONDS_PER_MOMENT && moment - current->lastSeen <= 20 * 60 / SECONDS_PER_MOMENT) {
                    if (!containsMacAddress(closeContacts, current->mac)) {
                        push(closeContacts, current);
                        closeContactsUpdated = 1;
                    }
                }
                current->lastSeen = moment;
                found = 1;
            }
        }
        if (found == 0) {
            struct contactentry *entry = createContactEntry(r, moment);
            push(recentAddresses, entry);
        }

        if (closeContactsUpdated == 1) {
            struct macaddress *macAddressArray = getMacAddressesArray(closeContacts);
            uploadContacts(macAddressArray, closeContacts->size);
            free(macAddressArray);
        }

        if (moment != 0 && moment % (4 * 60 * 60 * 10 / TIME_FACTOR) == 0) {
            bool testResult = testCOVID();
//            printf("[DEBUG] Test = %d\n", testResult);
            writeContactsToFile(closeContacts, testResult, moment);
        }

        double currentTime = getCurrentTimeInSeconds();
        double targetSleepSeconds = nextCycleTargetTime - currentTime;
        nextCycleTargetTime += SECONDS_PER_MOMENT / TIME_FACTOR;
        if (targetSleepSeconds > 0) {
//            printf("[DEBUG] Moment %012llu - SLEEPING FOR %lf\n", moment, targetSleepSeconds);
            fprintf(delaysFile, "%lf\n", targetSleepSeconds * TIME_FACTOR);
            usleep(targetSleepSeconds * 1000000);
        } else {
//            printf("[DEBUG] Moment %012llu - Skipping sleep, target sleep seconds: %lf\n", moment, targetSleepSeconds);
            fprintf(delaysFile, "%lf\n", 0.0);
        }
        moment++;
    }

    fclose(timeFile);
    fclose(delaysFile);
    free(addresses);
    return 0;
}
