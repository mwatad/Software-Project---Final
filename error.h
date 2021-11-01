/*
 * error.h
 *
 *  Created on: Aug 13, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>

/**
 * 	exit_if_no_memory: exits if ptr is a NULL pointer
 *
 *
 * @param ptr - the pointer to be checked
 *
 * @return
 *
 */
void exit_if_no_memory(void *ptr);

/**
 * 	exit_if_file_failure: prints an error and exits
 * 		if file is a NULL pointer
 *
 *
 * @param file - the file to be checked
 *
 * @return
 *
 */
void exit_if_file_failure(FILE *file);

/**
 * 	exit_if_read_write_failed: prints an error and exits a != b.
 *
 * @param a - first number
 * @param b - second number
 *
 * @return
 *
 */
void exit_if_read_write_failed(int a, int b);

/**
 * 	exit_if_dividing_by_zero: prints an error
 * 		and exits if m is zero
 *
 * @param m - the number to be checked
 *
 * @return
 *
 */
void exit_if_dividing_by_zero(double m);

/**
 * 	exit_insufficient_arguments: prints an error
 * 		and exits
 *
 * @return
 *
 */
void exit_insufficient_arguments();

/**
 * 	exit_insufficient_arguments: prints an error
 * 		and exits
 *
 * @return
 *
 */
void exit_infinite_loop_PI();

#endif /* ERROR_H_ */
