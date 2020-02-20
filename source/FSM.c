#include "FSM.h"
#include "hardware.h"
#include "timer.h"
#include "queue.h"

int elevator_initialize(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
        return -1;
    }

    for(int i = 0; i < 4; ++i){
        if (hardware_read_floor_sensor(i) == 1){
            return i;
        }
    }

    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    int j=0; // {0} vs = 0?
    while(1){ //greit med % (restarte på stort tall? automatisk?), og hva med timeout?
        if(hardware_read_floor_sensor((j % 4) == 1)){
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            return j;
        }
        ++j;
    }
}

void elevator_run() {
    //semiglobale variabler (pekere?)
    int floor_current = elevator_initialize();
    int direction = 0;
    int floor_next = -1; //get_next() må returnere -1 hvis køen er tom? underetasje
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
                for(int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; ++i) {
                    queue_remove(i);
                    if(hardware_read_floor_sensor(i) == 1) {
                        at_floor = 1;
                    }
                }
                hardware_command_door_open(at_floor);
                if(hardware_read_stop_signal() == 0) {
                    hardware_command_stop_light(0);
                    if(at_floor) {
                        door_close_time = time_get_close();
                        state = DOOR_OPEN;
                    }
                    else {
                        state = IDLE;
                    }
                }
                break;
            }

            case IDLE: {
                floor_next = queue_get_next(direction, floor_current);
                if(floor_next != -1) {
                    if(floor_next == floor_current) {
                        if(hardware_read_floor_sensor(floor_next)) {
                            hardware_command_order_light(floor_next, HARDWARE_ORDER_UP, 0);
                            hardware_command_order_light(floor_next, HARDWARE_ORDER_INSIDE, 0);
                            hardware_command_order_light(floor_next, HARDWARE_ORDER_DOWN, 0);
                            state = DOOR_OPEN;
                        }
                        else {
                            if(direction == 1) {
                                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                            }
                            else if (direction == 0) {
                                hardware_command_movement(HARDWARE_MOVEMENT_UP);
                            }
                            state = MOVING;
                        }
                        
                    }
                    else if(floor_next > floor_current) {
                        direction = 1;
                        hardware_command_movement(HARDWARE_MOVEMENT_UP);
                    }
                    else if(floor_next < floor_current) {
                        direction = 0;
                        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                    }
                }
            }

            case MOVING: {
                int j = 0;
                while(1) {
                    if(hardware_read_floor_sensor(j % 4)) {
                        floor_current = j % 4;
                        hardware_command_floor_indicator_on(floor_current);
                        if(hardware_read_floor_sensor(floor_current) == floor_next) {
                            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                            queue_remove(floor_current);

                            hardware_command_order_light(floor_next, HARDWARE_ORDER_UP, 0);
                            hardware_command_order_light(floor_next, HARDWARE_ORDER_INSIDE, 0);
                            hardware_command_order_light(floor_next, HARDWARE_ORDER_DOWN, 0);

                            door_close_time = time_get_close();
                            state = DOOR_OPEN;
                        }
                    }
                    ++j;
                }
            }
            
            case DOOR_OPEN: {
                hardware_command_door_open(1);
                if(hardware_read_obstruction_signal() == 1) {
                    door_close_time = time_get_close();
                }
                if(time_get_current() >= time_get_close()) {
                    hardware_command_door_open(0);
                    state = IDLE;
                }
            }
        }
    }
}

//ALLTID
//Sjekke stoppknapp
//Legge til bestillinger, sette lys.


//switch med tilstander, KUN VED GITT TILSTAND

//CASE STOP: stopp all bevegelse, slett køen, åpne dørene hvis i etasje,
//bytt til DOOR_OPEN når knappen slippes hvis i etasje, bytt til IDLE hvis ikke

//case IDLE: spør om det ligger noe i køen
//hvis ja, sett direction, bytt state til MOVING.

//case MOVING: poll etter etasjen heisen skal til, sett lys når den passerer etasjer
//når ankommet riktig etasje, stopp bevegelse, skru av lys, finn tidspunkt
//for å lukke dørene og bytt til state DOOR_OPEN

//case DOOR_OPEN: åpne dørene, poll etter tidspunkt å lukke dørene, 
//ta hensyn til obstruksjon og bytt til state IDLE når dørene er lukket
