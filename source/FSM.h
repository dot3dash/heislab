#ifndef FSM_H
#define FSM_H

// #define TRUE 0, FALSE !(TRUE)?

typedef enum {
    MOVING,
    IDLE,
    DOOR_OPEN,
    STOPPED
} ElevatorState;

int elevator_initialize();

void elevator_run(); 

#endif