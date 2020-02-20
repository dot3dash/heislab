#include <stdio.h>
#include <stdlib.h>

#define HARDWARE_NUMBER_OF_FLOORS 4

int up_queue[HARDWARE_NUMBER_OF_FLOORS ];
int down_queue[HARDWARE_NUMBER_OF_FLOORS];

//direction; 0=ned, 1=opp

void add_to_queue(int direction, int floor){
    if (direction == 0){
        down_queue[floor]=1;
    }

    else if (direction == 1) {
        up_queue[floor]=1;
    }
}


void remove_from_queue(int floor){
    down_queue[floor]=0;
    up_queue[floor]=0;
}

int get_next_floor(int direction, int current_floor){
    int next;
    if(direction == 0){ 
        for (int i=current_floor; i > 1; i--){
            if(down_queue[i]==1){
                return i+1;
            }
        }
        
    }
    else if(direction == 1){
         for (int i=0; i < HARDWARE_NUMBER_OF_FLOORS-current_floor; i++){
            if(up_queue[i]==1){
                return i+1;
            }
        }
    }
    else{
        return current_floor;
    }
}