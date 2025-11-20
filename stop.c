#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stop.h"

static Stop stops[MAX_STOPS];
static int stopCount = 0;
static int passengerIdCounter = 1;

void stop_manager_init(void) {
    for (int i = 0; i < MAX_STOPS; ++i) {
        stops[i].front = stops[i].rear = NULL;
        stops[i].id = -1;
        stops[i].name[0] = '\0';
    }
    stopCount = 0;
    passengerIdCounter = 1;
}

int add_stop(const char* name) {
    if (stopCount >= MAX_STOPS) {
        fprintf(stderr, "Maximum stop limit reached!\n");
        return -1;
    }
    stops[stopCount].id = stopCount;
    strncpy(stops[stopCount].name, name, MAX_NAME_LEN-1);
    stops[stopCount].name[MAX_NAME_LEN-1] = '\0';
    stops[stopCount].front = stops[stopCount].rear = NULL;
    printf("Added Stop %s (ID: %d)\n", stops[stopCount].name, stopCount);
    return stopCount++;
}

int get_stop_count(void) {
    return stopCount;
}

const char* get_stop_name(int stopId) {
    if (stopId < 0 || stopId >= stopCount) return NULL;
    return stops[stopId].name;
}

void enqueue_passenger_at_stop(int stopId, int destination) {
    if (stopId < 0 || stopId >= stopCount) {
        fprintf(stderr, "Invalid stop id %d\n", stopId);
        return;
    }
    Passenger* newP = (Passenger*)malloc(sizeof(Passenger));
    if (!newP) {
        perror("malloc");
        return;
    }
    newP->id = passengerIdCounter++;
    newP->destination = destination;
    newP->next = NULL;

    Stop* s = &stops[stopId];
    if (s->rear == NULL) {
        s->front = s->rear = newP;
    } else {
        s->rear->next = newP;
        s->rear = newP;
    }
    printf("Passenger %d added at Stop %s (Destination: %d)\n",
           newP->id, s->name, destination);
}

Passenger* dequeue_passenger_from_stop(int stopId) {
    if (stopId < 0 || stopId >= stopCount) {
        return NULL;
    }
    Stop* s = &stops[stopId];
    if (s->front == NULL) return NULL;
    Passenger* temp = s->front;
    s->front = s->front->next;
    if (s->front == NULL) s->rear = NULL;
    temp->next = NULL; /* detach */
    return temp;
}

void print_waiting_list(void) {
    printf("\n--- Waiting Passengers at Each Stop ---\n");
    for (int i = 0; i < stopCount; ++i) {
        printf("Stop %s: ", stops[i].name);
        Passenger* p = stops[i].front;
        if (!p) {
            printf("No one waiting\n");
            continue;
        }
        while (p) {
            printf("[P%d -> Dest %d] ", p->id, p->destination);
            p = p->next;
        }
        printf("\n");
    }
}