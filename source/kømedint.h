#include <stdio.h>
#include <stdlib.h>

#define HARDWARE_NUMBER_OF_FLOORS 4

int up_queue[HARDWARE_NUMBER_OF_FLOORS ];
int down_queue[HARDWARE_NUMBER_OF_FLOORS];

void add_to_queue(int direction, int floor);
void remove_from_queue(int floor);
int get_next_floor(int direction, int current_floor);
