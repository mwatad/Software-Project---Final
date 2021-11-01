/*
 * config.h
 *
 *  Created on: Sep 16, 2020
 *      Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#define EPSILON 0.00001
#define IS_POSITIVE(X) ((X) > EPSILON)
#define LESS_THAN(X, Y) IS_POSITIVE((Y)-(X))
#define GREATER_THAN(X, Y) IS_POSITIVE((X)-(Y))
#define EQUALS(X, Y) (((X)-(Y)) > (-EPSILON)) && (((X)-(Y)) < EPSILON)
#define PI_LIMIT(N) (3000*(N) + 400000)


#endif /* CONFIG_H_ */
