/**
 * @file
 * @brief A simple library for getting time used for timing
 */

#ifndef TIMER_H
#define TIMER_H

#define TIME_OPEN 3

/**
 * @brief Getting the time in TIME_OPEN seconds.
 *
 * @return the current time + TIME_OPEN seconds.
 */
unsigned long int time_get_close();

/**
 * @brief Getting the current time.
 *
 * @return the current time as an unsigned long int.
 */
unsigned long int time_get_current();

#endif