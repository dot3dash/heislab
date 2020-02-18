//#include "timer.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int const TIME_OPEN = 3; //dette vs #define TIME_OPEN 3

time_t getCloseTime() {
    return time(NULL) + TIME_OPEN;
}

time_t getCurrentTime() {
    return time(NULL);
}

int main() {
    /*
    printf("Tid nå: \%ld\n", getCurrentTime());
    printf("Tid om 3 sekunder: \%ld\n", getCloseTime());
    */
    time_t close = getCloseTime();
    printf("Åpner!\n");
    while (getCurrentTime() < close) {
        printf("Ikke ennå!\n");
        usleep(300000);
    }
    printf("Lukker!\n");

    return 0;
}