#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "bus.h"
#include "stop.h"
#include "trip_history.h"
#include "types.h"

/* buses are internal to this module */
static Bus buses[MAX_BUSES];
static int busCount = 0;

void bus_manager_init(void) {
    for (int i = 0; i < MAX_BUSES; ++i) {
        buses[i].id = -1;
        buses[i].passengers = NULL;
    }
    busCount = 0;
}

int add_bus(int capacity, const int* route, int routeLength) {
    if (busCount >= MAX_BUSES) {
        fprintf(stderr, "Maximum bus limit reached!\n");
        return -1;
    }
    if (routeLength <= 0 || routeLength > MAX_STOPS) {
        fprintf(stderr, "Invalid route length\n");
        return -1;
    }
    Bus* b = &buses[busCount];
    b->id = busCount;
    b->capacity = capacity;
    b->passengerCount = 0;
    b->passengers = NULL;
    b->currentStop = route[0];
    b->routeLength = routeLength;
    b->routeIndex = 0;
    for (int i = 0; i < routeLength; ++i) b->route[i] = route[i];
    printf("Added Bus %d (Capacity: %d)\n", b->id, b->capacity);
    return busCount++;
}

void handle_passengers(int busId) {
    if (busId < 0 || busId >= busCount) return;
    Bus* b = &buses[busId];
    int curStop = b->currentStop;
    const char* stopName = get_stop_name(curStop);

    /* Passengers exit */
    Passenger* prev = NULL;
    Passenger* curr = b->passengers;
    while (curr) {
        if (curr->destination == curStop) {
            printf("Passenger %d exited Bus %d at Stop %s\n", curr->id, b->id, stopName ? stopName : "Unknown");
            if (prev == NULL)
                b->passengers = curr->next;
            else
                prev->next = curr->next;

            Passenger* tmp = curr;
            curr = curr->next;
            free(tmp);
            b->passengerCount--;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }

    /* Passengers board from stop queue while bus has capacity */
    while (b->passengerCount < b->capacity) {
        Passenger* p = dequeue_passenger_from_stop(curStop);
        if (!p) break;
        /* add to front of onboard linked list */
        p->next = b->passengers;
        b->passengers = p;
        b->passengerCount++;
        printf("Passenger %d boarded Bus %d\n", p->id, b->id);
    }
}

void move_bus(int busId) {
    if (busId < 0 || busId >= busCount) return;
    Bus* b = &buses[busId];
    b->routeIndex = (b->routeIndex + 1) % b->routeLength;
    b->currentStop = b->route[b->routeIndex];
    const char* stopName = get_stop_name(b->currentStop);
    printf("\nBus %d moved to Stop %s\n", b->id, stopName ? stopName : "Unknown");
}

void free_all_buses(void) {
    for (int i = 0; i < busCount; ++i) {
        Passenger* p = buses[i].passengers;
        while (p) {
            Passenger* tmp = p->next;
            free(p);
            p = tmp;
        }
        buses[i].passengers = NULL;
    }
    busCount = 0;
}

/* Find shortest route between two stops using BFS across all bus routes */
void find_shortest_route(int startStop, int endStop) {
    printf("\n--- Finding Shortest Route from Stop %s to Stop %s ---\n",
           get_stop_name(startStop), get_stop_name(endStop));

    if (startStop < 0 || startStop >= get_stop_count() ||
        endStop < 0 || endStop >= get_stop_count()) {
        printf("Invalid stop IDs\n");
        return;
    }

    if (startStop == endStop) {
        printf("Already at destination!\n");
        return;
    }

    /* BFS data structures */
    int visited[MAX_STOPS] = {0};
    int parent[MAX_STOPS];
    int busUsed[MAX_STOPS];
    int queue[MAX_STOPS];
    int front = 0, rear = 0;

    for (int i = 0; i < MAX_STOPS; ++i) {
        parent[i] = -1;
        busUsed[i] = -1;
    }

    /* Start BFS */
    queue[rear++] = startStop;
    visited[startStop] = 1;

    int found = 0;
    while (front < rear && !found) {
        int current = queue[front++];

        /* Check all buses to see if they can take us from current stop */
        for (int b = 0; b < busCount; ++b) {
            Bus* bus = &buses[b];
            int currentInRoute = -1;

            /* Find if current stop is in this bus's route */
            for (int i = 0; i < bus->routeLength; ++i) {
                if (bus->route[i] == current) {
                    currentInRoute = i;
                    break;
                }
            }

            if (currentInRoute == -1) continue;

            /* Check all stops reachable from current on this bus */
            for (int i = 0; i < bus->routeLength; ++i) {
                int nextStop = bus->route[i];
                if (!visited[nextStop] && nextStop != current) {
                    visited[nextStop] = 1;
                    parent[nextStop] = current;
                    busUsed[nextStop] = b;
                    queue[rear++] = nextStop;

                    if (nextStop == endStop) {
                        found = 1;
                        break;
                    }
                }
            }
            if (found) break;
        }
    }

    if (!found) {
        printf("No route found from Stop %s to Stop %s\n",
               get_stop_name(startStop), get_stop_name(endStop));
        return;
    }

    /* Reconstruct path */
    int path[MAX_STOPS];
    int pathBuses[MAX_STOPS];
    int pathLen = 0;

    int curr = endStop;
    while (curr != -1) {
        path[pathLen] = curr;
        pathBuses[pathLen] = busUsed[curr];
        pathLen++;
        curr = parent[curr];
    }

    /* Print route in forward order */
    printf("Shortest route found (%d stops):\n", pathLen);
    for (int i = pathLen - 1; i >= 0; --i) {
        printf("  Stop %s", get_stop_name(path[i]));
        if (i > 0 && pathBuses[i-1] != -1) {
            printf(" -> [Bus %d] -> ", pathBuses[i-1]);
        }
    }
    printf("\n");
}

/* Display detailed status of a specific bus */
void display_bus_status(int busId) {
    if (busId < 0 || busId >= busCount) {
        printf("Invalid bus ID: %d\n", busId);
        return;
    }

    Bus* b = &buses[busId];
    printf("\n--- Bus %d Status ---\n", b->id);
    printf("Capacity: %d\n", b->capacity);
    printf("Current Passengers: %d\n", b->passengerCount);
    printf("Current Stop: %s (ID: %d)\n", 
           get_stop_name(b->currentStop), b->currentStop);
    printf("Route Index: %d/%d\n", b->routeIndex, b->routeLength - 1);

    printf("Route: ");
    for (int i = 0; i < b->routeLength; ++i) {
        if (i == b->routeIndex) printf("[");
        printf("%s", get_stop_name(b->route[i]));
        if (i == b->routeIndex) printf("]");
        if (i < b->routeLength - 1) printf(" -> ");
    }
    printf("\n");

    printf("Passengers on board:\n");
    if (b->passengers == NULL) {
        printf("  (none)\n");
    } else {
        Passenger* p = b->passengers;
        while (p) {
            printf("  Passenger %d (Destination: %s)\n",
                   p->id, get_stop_name(p->destination));
            p = p->next;
        }
    }
    printf("---------------------\n");
}