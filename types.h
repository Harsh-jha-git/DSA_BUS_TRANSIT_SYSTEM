#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

#define MAX_STOPS 10
#define MAX_BUSES 5
#define MAX_NAME_LEN 20

typedef struct Passenger {
    int id;                     // Unique passenger ID
    int destination;            // Destination stop ID
    struct Passenger* next;     // Next passenger (for linked lists / queues)
} Passenger;

typedef struct Stop {
    int id;
    char name[MAX_NAME_LEN];
    Passenger* front;
    Passenger* rear;
} Stop;

typedef struct Bus {
    int id;
    int capacity;
    int passengerCount;
    int currentStop;            // stop index
    int route[MAX_STOPS];
    int routeLength;
    int routeIndex;
    Passenger* passengers;      // linked list of onboard passengers
} Bus;

typedef struct TripHistory {
    int busId;
    int totalTrips;
    struct TripHistory* next;
} TripHistory;

#endif /* TYPES_H */