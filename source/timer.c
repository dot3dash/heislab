#include "timer.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

unsigned long int time_get_close() {
    return time(NULL) + TIME_OPEN;
}

unsigned long int time_get_current() {  //snake, timeGetCurrent!
    return time(NULL);
}