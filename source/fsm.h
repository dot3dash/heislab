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
 *
 */
int elevator_initialize();

void elevator_run(); 

#endif