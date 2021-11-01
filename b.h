/*
 * b.h
 *
 *  Created on: Aug 14, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef B_H_
#define B_H_

#include "a.h"

/*
 * modes of multiply operations
 */
typedef enum {normal, hat, shifted, b_f} b_mode;

/*
 * status data type for unmoved array
 */
typedef enum {unmoved, moved} status;

/**
 * This struct represents B[g] hat matrix.
 *
 * @field a  - A[g] matrix
 * @field g - the group g that defines B[g]
 * @field n_g - size of g
 * @field f - sums of rows of B[g]
 * @field C - 1-norm of B
 */
typedef struct B_t {
	A *a;
	int *g;
	int n_g;
	double *f;
	double C;
} B;

/**
 *	B_create: Creates B[g] hat matrix based on sub matrix a of A,
 *	group of vertices g, the size of g and a the 1-norm of B
 *
 * @param a  - sparse matrix
 * @param g  - group of vertices
 * @param n_g - size of g
 * @param norm - norm of B
 *
 * @return
 * 		B[g] hat matrix
 */
B* B_create(A *a, int *g, int n_g, double norm);

/**
 *	B_destroy: deallocate memory that was used
 *	for B[g] hat matrix
 *
 * @param b - B[g] hat matrix to be destroyed
 *
 * @return
 */
void B_destroy(B *b);

/**
 *	B_initialize_g: initializes the group of vertices
 *					that defines a B[g] hat matrix
 *
 * @param b - B[g] hat matrix to be initialized
 *
 * @return
 */
void B_initialize_g(B *b);

/**
 *	B_get_ng: returns the size of the group g that defines
 *			  a B[g] hat matrix
 *
 * @param b - B[g] hat matrix
 *
 * @return
 * 		the size of the group g that defines b
 */
int B_get_ng(B *b);

/**
 *	B_calculate_f: calculates array f
 *				   (sums of a B[g] hat matrix rows)
 *
 * @param b - B[g] hat matrix
 *
 * @return
 */
void B_calculate_f(B *b);

/**
 *	B_calculate_1_norm: calculates 1-norm of a B[g] hat matrix
 *
 * @param b - B[g] hat matrix
 *
 * @return
 */
void B_calculate_1_norm(B *b);

/**
 *	B_multiply: calculates the multiplication of b with vector v,
 *				and writes the result to result array.
 *
 * @param b - B[g] hat matrix
 * @param v - vector array
 * @param result - result array
 * @param q - mode of multiplication:
 * 			  normal: calculates multiplication of B[g]
 * 			  hat: calculates multiplication of B[g] hat
 * 			  shifted: calculates multiplication of B[g] hat shifted
 *
 * @return
 * 		the norm of result vector
 */
double B_multiply(B *b, double *v, double *result, b_mode q);

/**
 * 	B_calculate_next: performs 1 iteration of the power iteration algorithm
 *
 * @param b - B[g] hat matrix
 * @param v_prev - the vector to be multiplied by b
 * @param v_next - result array
 *
 * @return
 * 		true: if each entry of v_next is EPSILON close
 * 			  to the corresponding entry in v_prev
 * 		false: otherwise
 */
boolean B_calculate_next(B *b, double *v_prev, double *v_next);

/**
 * 	B_find_leading_eigenpair: calculates the leading eigenvalue and eigenvector
 * 		of a B[g] hat shifted matrix. It uses aux_array as an auxiliary array
 * 		while computing. The eigenvector is written to leading_v, and the
 * 		eigenvalue is returned.
 *
 * @param b - B[g] hat matrix
 * @param leading_v - result array which is intended to store the eigenvector
 * @param aux_array - auxiliary array
 *
 * @return
 * 		eigenvalue of b (of type B[g] hat)
 */
double B_find_leading_eigenpair(B *b, double *leading_v, double *aux_array);

/**
 * 	B_divide_into_two_groups: implements algorithm 2 which is described in the
 * 		project document: modifies leading_v according to the sign of the vector
 * 		entries (positive entries are mapped to 1, non-positive entries are mapped to
 * 		-1). if b's eigenvalue or the modularity is not-positive then leading_v's entries
 * 		are all set to 1 (trivial partition).
 *
 * @param b - B[g] hat matrix
 * @param leading_v - result array which represents the partition
 * @param aux_array - auxiliary array
 *
 * @return
 */
void B_divide_into_two_groups(B *b, double *leading_v, double *aux_array);

/**
 * 	B_divide_into_two_groups: implements algorithm 4 which is described in the
 * 		project document: starts from a division which is derived from vector s
 * 		and tries to improve the division.
 *
 * @param b - B[g] hat matrix
 * @param b1 - B[g] hat matrix that represents one part of the division
 * @param b2 - B[g] hat matrix that represents the other part of the division
 * @param b2 - B[g] hat matrix that represents the other part of the division
 * @param s - the initial division vector
 * @param temp - auxiliary array
 * @param bs - auxiliary array
 * @param indices - auxiliary array
 * @param unmoved_arr - auxiliary array
 *
 * @return
 */
void B_maximize_modularity(B **b, B **b1, B **b2, double *s, double *temp,
		double *bs,int *indices, status *unmoved_arr);

/**
 * 	B_calculate_modularity: calculates the modularity sBs and use res
 * 		array as an auxiliary array.
 *
 * @param b - B[g] hat matrix
 * @param s - division array
 * @param res - result array
 *
 * @return
 * 		The modularity - sBs
 */
double B_calculate_modularity(B *b, double *s, double *res);


#endif /* B_H_ */
