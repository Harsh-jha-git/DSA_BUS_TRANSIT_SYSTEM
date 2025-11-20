#include <stdio.h>
#include <stdlib.h>
#include "trip_history.h"

static TripHistory* historyHead = NULL;

void trip_history_init(void) {
    /* free any existing history (defensive) */
    TripHistory* cur = historyHead;
    while (cur) {
        TripHistory* tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    historyHead = NULL;
}

void record_trip(int busId) {
    TripHistory* temp = historyHead;
    while (temp) {
        if (temp->busId == busId) {
            temp->totalTrips++;
            return;
        }
        temp = temp->next;
    }
    TripHistory* newEntry = (TripHistory*)malloc(sizeof(TripHistory));
    if (!newEntry) {
        perror("malloc");
        return;
    }
    newEntry->busId = busId;
    newEntry->totalTrips = 1;
    newEntry->next = historyHead;
    historyHead = newEntry;
}

void generate_report(void) {
    printf("\n--- Trip History Report ---\n");
    TripHistory* temp = historyHead;
    if (!temp) {
        printf("No trips recorded.\n");
        return;
    }
    while (temp) {
        printf("Bus %d completed %d trips\n", temp->busId, temp->totalTrips);
        temp = temp->next;
    }
}