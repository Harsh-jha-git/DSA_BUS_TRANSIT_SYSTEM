#ifndef TRIP_HISTORY_H
#define TRIP_HISTORY_H
#include "types.h"


/* Trip history API */
void trip_history_init(void);
void record_trip(int busId);
void generate_report(void);

#endif /* TRIP_HISTORY_H */