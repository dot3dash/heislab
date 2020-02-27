#include "queue.h"

int queue_up[HARDWARE_NUMBER_OF_FLOORS];
int queue_down[HARDWARE_NUMBER_OF_FLOORS];

void queue_add(QueueDirection direction, int floor_current){
    if (direction == QUEUE_UP) {
        queue_up[floor_current]=1;
    }
    if(direction == QUEUE_BOTH) {
    	queue_down[floor_current] = 1;
	queue_up[floor_current] = 1;
    }
    if (direction == QUEUE_DOWN){
        queue_down[floor_current]=1;
    }
}

void queue_remove(int floor){
    queue_down[floor]=0;
    queue_up[floor]=0;
}

int queue_get_next(int direction, int floor_current){
    if(direction == 0){ 
        for (int i=floor_current; i >= 0; i--){
            if(queue_down[i]==1){
                return i;
            }
        }
         for (int i=0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
            if(queue_up[i]==1){
                return i;
            }
        }
        for (int i=HARDWARE_NUMBER_OF_FLOORS-1; i >= 0; i--){
            if(queue_down[i]==1){
                return i;
            }
        }
        
    }
    if(direction == 1){
         for (int i=floor_current; i < HARDWARE_NUMBER_OF_FLOORS; i++){
            if(queue_up[i]==1){
                return i;
            }
        }
        for (int i=HARDWARE_NUMBER_OF_FLOORS-1; i >= 0; i--){
            if(queue_down[i]==1){
                return i;
            }
        }
        for (int i=0; i < HARDWARE_NUMBER_OF_FLOORS; i++){
            if(queue_up[i]==1){
                return i;
            }
        }

    }
    return -1;
}