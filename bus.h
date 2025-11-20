#ifndef BUS_H
#define BUS_H

#include "types.h"

/* Bus manager API */
void bus_manager_init(void);
int add_bus(int capacity, const int* route, int routeLength); /* returns bus id or -1 */
void handle_passengers(int busId);
void move_bus(int busId);
void free_all_buses(void);

/* New functions */
void find_shortest_route(int startStop, int endStop);
void display_bus_status(int busId);

#endif /* BUS_H */