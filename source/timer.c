#include "timer.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define TIME_OPEN 3

//int const TIME_OPEN = 3; //dette vs #define TIME_OPEN 3

time_t time_get_close() {
    return time(NULL) + TIME_OPEN;
}

time_t time_get_current() {  //snake, timeGetCurrent!
    return time(NULL);
}