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
 * @brief A function with and infinite loop that will run the elevator.
 * Contains the logic of the fsm and uses the other modules to get the floor
 * order and timing of the door, and access to the hardware.
 */
void elevator_run();

#endif