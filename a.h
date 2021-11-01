/*
 * a.h
 *
 *  Created on: Aug 14, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef A_H_
#define A_H_

typedef enum {false, true} boolean;
typedef enum {f, mult} a_mode;

/**
 * This struct represents a sparse matrix data type.
 * The matrix is represented according to the array
 * implementation from HW2.
 *
 * @field n  - matrix order
 * @field colind - the indices of the columns of the non-zero values
 * 			 in the sparse matrix
 * @field rowptr - contains the indices of the first non-zero value
 * 			 of each row of the sparse matrix
 * @field ind_array - contains the indices of the first non-zero value
 * 			 of each row of the sparse matrix after being mapped to
 * 			 continues indices staring from zero
 * @field first_empty - the index of the first empty cell in colind array
 * @field m - sum of the ranks of the vertices of the original graph
 * @field nnz - number of the non-zero elements in the sparse matrix
 * @field k - ranks array of the vertices from the original array
 * @field touched - a flag which indicates if a multiply operation was
 * 					performed on the sparse matrix
 */
typedef struct A_t {

	/* Matrix size (n*n) */
	int		n;
	int *colind;
	int *rowptr;
	int *ind_array;
	int first_empty;
	int m;
	int nnz;
	int *k;
	boolean touched;

} A;



/**
 *	A_create: Creates a sparse matrix which
 *	has n rows and m non-zero entries.
 *
 * @param n  - matrix order
 * @param m  - number of non-zero elements
 *
 * @return
 * 		A sparse n x n matrix which has m non-zero entries
 */
A* A_create(int n, int m);


/**
 *	A_split: Creates sub sparse matrix which is defined
 *	over g1 from a sparse matrix a that is defined over g.
 *
 * @param a  - sparse matrix to be splitted
 * @param g  - group of vertices
 * @param n  - size of g
 * @param g1 - sub group of g
 * @param n1 - sizf of g1
 *
 * @return
 * 		A sub sparse matrix of matrix a which is defined over g1
 */
A* A_split(A* a, int *g, int n, int *g1, int n1);

/**
 *	A_destroy: deallocate memory that was used
 *	for a sparse matrix a
 *
 * @param a - sparse matrix to be freed
 *
 * @return
 */
void A_destroy(A *a);

/**
 *	A_add_row: fills row i of a previously created sparse matrix a
 *
 * @param a - sparse matrix to be updated
 * @param row - elements to be added to row i of sparse matrix a
 * @param size - num of elements to be added to row i
 * @param i - the index of the row of sparse matrix a to be filled
 *
 * @return
 */
void A_add_row(A *a, const int *row, int size, int i);

/**
 *	A_add_row_g: initializes row i of a previously created sub sparse matrix a
 *	with elements from address i_first to address i_last that belongs to g
 *
 * @param a_g - sub sparse matrix to be updated
 * @param i_first - address of first element
 * @param i_last - address of last element
 * @param g - the group that defines a_g sub sparse matrix
 * @param i - the index of the row of a_g sparse matrix a to be filled
 *
 * @return
 */
void A_add_row_g(A *a_g, int *i_first, int *i_last, int *g, int i);

/**
 *	A_multiply: calculates the multiplication of a sparse
 *	matrix a which is defined over g of size n_g with vector v,
 *	and writes the result to result array.
 *
 * @param a - sparse matrix
 * @param v - vector array
 * @param result - result array
 * @param g - the group that defines a_g sub sparse matrix
 * @param n_g - size of g
 * @param m - mode of operation
 *
 * @return
 */
void A_multiply(A* a, double *v, double *res, int *g, int n_g, a_mode m);

/**
 *	A_slow_multiply: performs a slow multiplication of a sparse
 *	matrix a which is defined over g of size n_g with vector v,
 *	and writes the result to result array.
 *
 * @param a_g - sparse matrix
 * @param v - vector array
 * @param res - result array
 * @param g - the group that defines a sub sparse matrix a_g
 * @param n_g - size of g
 * @param m - mode of operation
 *
 * @return
 */
void A_slow_multiply(A* a_g, double *v, double *res, int *g, int n_g, a_mode m);

/**
 *	A_fast_multiply: performs a fast multiplication of a sparse
 *	matrix a which is defined over a group of size n_g with vector v,
 *	and writes the result to result array.
 *
 * @param a_g - sparse matrix
 * @param v - vector array
 * @param result - result array
 * @param n_g - size of the group that defines a_g
 * @param m - mode of operation
 *
 * @return
 */
void A_fast_multiply(A* a_g, double *v, double *result, int n_g, a_mode m);

/**
 *	A_print: prints a sparse matrix a
 *
 * @param a - sparse matrix to be printed
 *
 * @return
 */
void A_print(A *a);


#endif /* A_H_ */
