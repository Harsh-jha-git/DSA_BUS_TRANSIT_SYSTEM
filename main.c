#include <stdio.h>
#include "types.h"
#include "stop.h"
#include "bus.h"
#include "trip_history.h"


int main(void) {
    /* Initialize managers */
    stop_manager_init();
    bus_manager_init();
    trip_history_init();

    /* Step 1: Create stops */
    add_stop("A");
    add_stop("B");
    add_stop("C");
    add_stop("D");

    /* Step 2: Add buses with routes */
    int route1[] = {0, 1, 2, 3};
    int bus0 = add_bus(3, route1, 4);

    int route2[] = {0, 2, 3};  /* Another bus with different route */
    int bus1 = add_bus(2, route2, 3);

    /* Step 3: Add waiting passengers */
    enqueue_passenger_at_stop(0, 2); /* at A want to go to C */
    enqueue_passenger_at_stop(0, 3); /* at A want to go to D */
    enqueue_passenger_at_stop(1, 3); /* at B want to go to D */
    enqueue_passenger_at_stop(2, 1); /* at C want to go to B */

    /* Step 4: Simulate bus movement & operations */
    for (int i = 0; i < 5; ++i) {
        handle_passengers(bus0);
        record_trip(bus0);
        move_bus(bus0);
    }

    /* Step 5: Display waiting lists and reports */
    print_waiting_list();
    generate_report();

    /* Step 6: Test new functions */
    printf("\n========== TESTING NEW FUNCTIONS ==========\n");
    
    /* Display bus status */
    display_bus_status(bus0);
    display_bus_status(bus1);
    
    /* Find shortest routes */
    find_shortest_route(0, 3);  /* A to D */
    find_shortest_route(1, 2);  /* B to C */
    find_shortest_route(0, 1);  /* A to B */
    find_shortest_route(2, 2);  /* C to C (same stop) */
    find_shortest_route(1, 10); /* Invalid stop */

    /* Cleanup dynamic memory */
    free_all_buses();

    return 0;
}