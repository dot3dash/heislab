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
 * Udtelt, trengs dox?
 */
static void sigint_handler(int sig);

/**
 * @brief Initializes the elevator with @c hardware_init(),
 * and takes the elevator downwards to a floor so it knows where it is.
 *
 * @return the floor it first reaches.
 */
int elevator_initialize();

/**
 * @brief Checks if the next order to @p floor_next is above the
 * elevators @p current_floor.
 *
 * @param floor_next The next floor in the queue.
 * @param floor_current The last floor the elevator reached.
 *
 * @return A truthy value if @p floor_next is above @p floor_current.
 */
int order_above(int floor_next, int floor_current);

/**
 * @brief Checks if the next order to @p floor_next is below the
 * elevators @p current_floor.
 *
 * @param floor_next The next floor in the queue.
 * @param floor_current The last floor the elevator reached.
 *
 * @return A truthy value if @p floor_next is below @p floor_current.
 */
int order_below(int floor_next, int floor_current);

/**
 * @brief Checks if the next order to @p floor_next is the same as the
 * elevators @p current_floor.
 *
 * @param floor_next The next floor in the queue.
 * @param floor_current The last floor the elevator reached.
 *
 * @return A truthy value if @p floor_next is the same as @p floor_current.
 */
int oder_at_floor(int floor_next, int floor_current);

/**
 * @brief 
 *
 */
void move_to_last(int elevator_direction, int floor_next, int* direction);

// Dokumenter etter at den ^ virker!






/**
 * @brief A function with and infinite loop that will run the elevator.
 * Contains the logic of the fsm and uses the other modules to get the floor
 * order and timing of the door, and access to the hardware.
 */
void elevator_run();

#endif