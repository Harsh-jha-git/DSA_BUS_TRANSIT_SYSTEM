#ifndef STOP_H
#define STOP_H

#include "types.h"

/* Stop manager API */
void stop_manager_init(void);
int add_stop(const char* name); /* returns stop id or -1 on error */
int get_stop_count(void);
const char* get_stop_name(int stopId);

/* Queue operations */
void enqueue_passenger_at_stop(int stopId, int destination);
Passenger* dequeue_passenger_from_stop(int stopId); /* returns passenger or NULL */

/* Display */
void print_waiting_list(void);

#endif /* STOP_H */