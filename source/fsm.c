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

int order_above(int floor_next, int floor_current) {
    return floor_next > floor_current;
}

int order_below(int floor_next, int floor_current) {
    return floor_next < floor_current;
}
int oder_at_floor(int floor_next, int floor_current) {
    return floor_next == floor_current;
}

void move_to_last(int elevator_direction, int floor_next) {
    if(!hardware_read_floor_sensor(floor_next)) {
        if(elevator_direction == 1){
            direction = 0;
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
        if(elevator_direction == 0) {
            direction = 1;
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
        }
    }
}

void elevator_run() {
    int floor_current = elevator_initialize();
    int direction = 0;
    int elevator_direction = 0;
    int floor_next = -1;
    unsigned long int door_close_time = time_get_close();
    hardware_command_floor_indicator_on(floor_current);
    ElevatorState state = IDLE;

    while(1) {
        if (hardware_read_stop_signal() == 1) {
            hardware_command_stop_light(1);
            state = STOPPED;
        }

        if(state != STOPPED) {
            for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i) {
                if(hardware_read_order(i, HARDWARE_ORDER_UP)) {
                    queue_add(1, i);
                    hardware_command_order_light(i, HARDWARE_ORDER_UP, 1);
                }
                if(hardware_read_order(i, HARDWARE_ORDER_INSIDE)) {
                    queue_add(2, i); //legg til 2 for begge i kømodulen
                    hardware_command_order_light(i, HARDWARE_ORDER_INSIDE, 1); 
                }
                if(hardware_read_order(i, HARDWARE_ORDER_DOWN)) {
                    queue_add(0, i);
                    hardware_command_order_light(i, HARDWARE_ORDER_DOWN, 1);
                }
            }
        }
        switch(state) {
            case STOPPED: {
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                int at_floor = 0;
                for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; ++f) {
                    queue_remove(f);
                    clear_all_floor_lights(f);
                    if(hardware_read_floor_sensor(f) == 1) {
                        at_floor = 1;
                        hardware_command_floor_indicator_on(f);
                        floor_current = f;
                    }
                }
                if(at_floor) {
                    hardware_command_door_open(at_floor);
                    door_close_time = time_get_close();
                    state = DOOR_OPEN;
			        break;
                }
                if(hardware_read_stop_signal() == 0) {
                    hardware_command_stop_light(0);
                }
                state = IDLE; //Merknad (ta med if (hardwarereadstop==0)?)
                break;
            }

            case IDLE: {
                floor_next = queue_get_next(direction, floor_current);
                if(floor_next == -1) {
                    break;
                }
                if(order_above(floor_next, floor_current)) {
                    direction = 1;
                    hardware_command_movement(HARDWARE_MOVEMENT_UP);
			        state = MOVING;
                    break;
                }
                if(order_below(floor_next, floor_current)) {
                    direction = 0;
                    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
			        state = MOVING;
                    break;
                }
                if(oder_at_floor(floor_next, floor_current)) {
                        move_to_last();
                        state = MOVING;
		                break; //merknad!
                }
                break;
            }

            case MOVING: {
		        floor_next = queue_get_next(direction, floor_current);	 
                for(int k = 0; k < HARDWARE_NUMBER_OF_FLOORS; k++) {
                    if(hardware_read_floor_sensor(k)) {
                        floor_current = k;
                        hardware_command_floor_indicator_on(floor_current);
                        elevator_direction = direction;
                    }
                }
                if(hardware_read_floor_sensor(floor_next)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    queue_remove(floor_current);
                    clear_all_floor_lights(floor_current);
                    door_close_time = time_get_close();
                    state = DOOR_OPEN;
                    break;
                }
                break;
            }
            
            case DOOR_OPEN: {
                hardware_command_door_open(1);
                floor_next = queue_get_next(direction, floor_current);
                if(floor_next == floor_current) {
                    clear_all_floor_lights(floor_current);
                    queue_remove(floor_current);
                    door_close_time = time_get_close();
                }
                if(hardware_read_obstruction_signal() == 1) {
                    door_close_time = time_get_close();
                }
                if(time_get_current() >= door_close_time) {
                    hardware_command_door_open(0);
                    state = IDLE;
                }
                break;
            }
        }
    }
}

int main(){

    signal(SIGINT, sigint_handler);
    elevator_run();

    return 0;
}