/**
 * @file
 * @brief A Finite State Machine to control the elevator.
 */
#ifndef FSM_H
#define FSM_H


/**
 * @brief The state types used by @c elevator_run().
 */
typedef enum {
    MOVING,
    IDLE,
    DOOR_OPEN,
    STOPPED
} ElevatorState;

/**
 * @brief Initializes the elevator with @c hardware_init(),
 * and takes the elevator downwards to a floor so it knows where it is.
 *
 * @return the floor it first reaches.
 */
int elevator_initialize();

int order_above(int floor_next, int floor_current);

int order_below(int floor_next, int floor_current);

int oder_at_floor(int floor_next, int floor_current);

void move_to_last(int elevator_direction, int floor_next, int* direction);

/**
 * @brief A function with and infinite loop that will run the elevator.
 * Contains the logic of the fsm and uses the other modules to get the floor
 * order and timing of the door, and access to the hardware.
 */
void elevator_run();

#endif