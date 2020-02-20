#include <stdio.h>
#include <stdlib.h>

#define HARDWARE_NUMBER_OF_FLOORS 4

int up_queue[HARDWARE_NUMBER_OF_FLOORS ];
int down_queue[HARDWARE_NUMBER_OF_FLOORS];


void queue_add(int direction, int floor);
void queue_remove(int floor);
int queue_get_next(int direction, int current_floor);
