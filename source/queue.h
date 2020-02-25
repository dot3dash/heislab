/**
 * @file
 * @brief A queue handler for the elevator.
 */

#include <stdio.h>
#include <stdlib.h>
#define HARDWARE_NUMBER_OF_FLOORS 4 //definere det i to filer, eller inkludere den andre?

/**
 * @brief Adding a floor to the queue.
 *
 * @param[in] direction The direction of the elevator.
 * @param[in] floor_current The current floor of the elevator.
 */
void queue_add(int direction, int floor_current);

/**
 * @brief Removing av floor from the queue.
 *
 * @param[in] floor The floor to be removed from the queue.
 */
void queue_remove(int floor);

/**
 * @brief Getting the next floor in the queue.
 *
 * @param[in] direction The direction of the elevator.
 * @param[in] floor_current The current floor of the elevator.
 *
 * @return The next floor in the queue, -1 if the queue is empty \emoji "hankey".
 */
int queue_get_next(int direction, int floor_current);
