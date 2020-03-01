#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "fsm.h"
#include "hardware.h"
#include "timer.h"
#include "queue.h"

static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

static void clear_all_floor_lights(int floor){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int i = 0; i < 3; i++){
        HardwareOrder type = order_types[i];
        hardware_command_order_light(floor, type, 0);
    }
}

int elevator_initialize(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i){
        if (hardware_read_floor_sensor(i) == 1){
            return i;
        }
    }

    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    while(1){
        for(int k = 0; k < HARDWARE_NUMBER_OF_FLOORS; k++) {
            if(hardware_read_floor_sensor(k) == 1) {
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                return k;
            }
        }
    }
}

void move_to_last(int* elevator_direction, int* floor_next, int* direction) {
    if(!hardware_read_floor_sensor(*floor_next)) {
        if(*elevator_direction == 1){
            *direction = 0;
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
        if(*elevator_direction == 0) {
            *direction = 1;
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
        }
    }
}

void state_stopped(ElevatorState* state, unsigned long int* door_close_time) {
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    hardware_command_stop_light(1);

	int to_door = 0;
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; ++f) {
        queue_remove(f);
        clear_all_floor_lights(f);
        if(hardware_read_floor_sensor(f) == 1) {
            *door_close_time = time_get_close();
            hardware_command_door_open(1);
            *state = DOOR_OPEN;
			to_door = 1;
        }
    }

    if(to_door) {
		return;
	}

    if(hardware_read_stop_signal() == 0) {
        *state = IDLE;
        return;
    }
}

void state_idle(ElevatorState* state, int* floor_next, int* floor_current, int* direction, int* elevator_direction) {
    if(*floor_next == -1) {
        return;
    }

    if(*floor_next > *floor_current) {
        *direction = 1;
        hardware_command_movement(HARDWARE_MOVEMENT_UP);
		*state = MOVING;
        return;
    }

    if(*floor_next < *floor_current) {
        *direction = 0;
        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
		*state = MOVING;
        return;
    }

    if(*floor_next == * floor_current) {
        move_to_last(elevator_direction, floor_next, direction);
        *state = MOVING;
		return;
    }
}

void state_moving(ElevatorState* state, int* floor_next, int* floor_current, int* direction, unsigned long int* door_close_time) {
    *floor_next = queue_get_next(*direction, *floor_current);

    if(hardware_read_floor_sensor(*floor_next)) {
        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        clear_all_floor_lights(*floor_current);
        queue_remove(*floor_current);

        *door_close_time = time_get_close();
        *state = DOOR_OPEN;
        return;
    }
}

void state_door_open(ElevatorState* state, int* floor_next, int* floor_current, int* direction, unsigned long int* door_close_time) {
    hardware_command_door_open(1);
    *floor_next = queue_get_next(*direction, *floor_current);

    if(*floor_next == *floor_current) {
        clear_all_floor_lights(*floor_current);
        queue_remove(*floor_current);
        *door_close_time = time_get_close();
        return;
    }

    if(hardware_read_obstruction_signal() == 1) {
        *door_close_time = time_get_close();
        return;
    }

    if(time_get_current() >= *door_close_time) {
        hardware_command_door_open(0);
        *state = IDLE;
        return;
    }
}

void elevator_run() {
    int floor_current = elevator_initialize();
    int direction = 0;
    int elevator_direction = 0;
    int floor_next = -1;
    ElevatorState state = IDLE;
    unsigned long int door_close_time = time_get_close();
    
    hardware_command_floor_indicator_on(floor_current);
    
    while(1) {
        if (hardware_read_stop_signal() == 1) {
            state = STOPPED;
        }
        else {
            hardware_command_stop_light(0);
        }

        if(state != STOPPED) {
            for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i) {
                if(hardware_read_order(i, HARDWARE_ORDER_UP)) {
                    queue_add(QUEUE_UP, i);
                    hardware_command_order_light(i, HARDWARE_ORDER_UP, 1);
                }
                if(hardware_read_order(i, HARDWARE_ORDER_INSIDE)) {
                    queue_add(QUEUE_BOTH, i);
                    hardware_command_order_light(i, HARDWARE_ORDER_INSIDE, 1); 
                }
                if(hardware_read_order(i, HARDWARE_ORDER_DOWN)) {
                    queue_add(QUEUE_DOWN, i);
                    hardware_command_order_light(i, HARDWARE_ORDER_DOWN, 1);
                }
            }
        }
        
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
            if(hardware_read_floor_sensor(f)) {
            floor_current = f;
            hardware_command_floor_indicator_on(floor_current);
            elevator_direction = direction;
            }
        }

        floor_next = queue_get_next(direction, floor_current);

        switch(state) {
            case STOPPED:
                state_stopped(&state, &door_close_time);
                break;

            case IDLE:
                state_idle(&state, &floor_next, &floor_current, &direction, &elevator_direction);
                break;

            case MOVING:
                state_moving(&state, &floor_next, &floor_current, &direction, &door_close_time);
                break;
            
            case DOOR_OPEN:
                state_door_open(&state, &floor_next, &floor_current, &direction, &door_close_time);
                break;
        }
    }
}

int main(){
    signal(SIGINT, sigint_handler);
    elevator_run();

    return 0;
}