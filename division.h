/*
 * division.h
 *
 *  Created on: Aug 13, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef DIVISION_H_
#define DIVISION_H_

#include "b.h"

/**
 * This struct represents a node in the division's linked list.
 * Each node contains a B[g] hat matrix.
 *
 * @field b - B[g] hat matrix
 * @field nest - pointer to next element
 *
 */
struct node_t {
	B *b;
	struct node_t *next;
};

typedef struct node_t Node;

/**
 * This struct represents division.
 *
 * @field first - pointer to the first element
 * @field last - pointer to the last element
 * @field size - number of groups in the divison
 *
 */
typedef struct division_t {

	Node *first;
	Node *last;

	int size;

} Division;


/**
 *	division_create: Creates a new division
 *
 * @return
 * 		new allocated division
 *
 */
Division* division_create();

/**
 *	division_destroy: deallocate a division
 *
 *	@param division - a division to be freed
 *
 *	return
 *
 */
void division_destroy(Division *division);

/**
 *	division_add_group: adds a B[g] hat matrix to the division
 *
 *	@param division - a division
 *	@param b - a B[g] hat matrix to be added to the division
 *
 *	return
 *
 */
void division_add_group(Division *division, B *b);

/**
 *	division_extract_group: extract a group from a divison
 *
 *	@param division - a division
 *
 *	return
 *		A group which was extracted
 */
B* division_extract_group(Division *division);

/**
 *	division_print: prints a divison
 *
 *	@param division - a division
 *
 *	return
 */
void division_print(Division *division);

/**
 *	division_get_size: return the size of a division
 *
 *	@param division - a division
 *
 *	return
 *		the size of the division
 */
int division_get_size(Division *division);


#endif /* DIVISION_H_ */
