/**
 * @file
 * @brief A queue noe annet for the elevator.
 */

#include <stdio.h>
#include <stdlib.h>
#define HARDWARE_NUMBER_OF_FLOORS 4

/**
 * @brief Adding a floor to the queue.
 *
 * @param direction The direction of the elevator,
 * 0 for queue_down, 1 for queue_up, and 2 for both.
 * @param floor_current The current floor of the elevator.
 */
void queue_add(int direction, int floor_current);

/**
 * @brief Removing av floor from the queue.
 *
 * @param floor The floor to be removed from the queue.
 */
void queue_remove(int floor);

/**
 * @brief Getting the next floor in the queue.
 *
 * @param direction The direction of the elevator.
 * @param floor_current The current floor of the elevator.
 *
 * @return The next floor in the queue, -1 if the queue is empty.
 */
int queue_get_next(int direction, int floor_current);