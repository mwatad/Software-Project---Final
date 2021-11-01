/*
 * a.c
 *
 *  Created on: Aug 14, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */



#include <stdlib.h>
#include "a.h"
#include "error.h"


/*
 * creating a sparse matrix
 */
A* A_create(int n, int m){

	A* a = (A*)malloc(sizeof(A));
	exit_if_no_memory(a);

	a->colind = (int*)malloc(m * sizeof(int));
	exit_if_no_memory(a->colind);

	a->rowptr = (int*)malloc((n + 1) * sizeof(int));
	exit_if_no_memory(a->rowptr);
	(a->rowptr)[n] = m;

	a->n = n;
	a->k = NULL;
	a->first_empty = 0;
	a->m = m;
	a->nnz = m;
	a->touched = false;

	return a;
}


/*
 * returns the number of shared elements between
 * two ascendingly sorted arrays
 */
int num_of_shared_values(int *arr1, int *arr1_last,
			int *arr2, int *arr2_last) {

	register int cnt = 0;

	while (arr1 < arr1_last && arr2 < arr2_last) {

		if (*arr1 < *arr2)
			arr1++;

		else if (*arr2 < *arr1)
			arr2++;

		else {
			arr1++;
			arr2++;
			cnt++;
		}
	}

	return cnt;
}



/*
 * fills rows of a sub sparse matrix
 */
void A_initialize_rows_of_sub_mat(A *a, int *g, int n,
		A *sub_mat, int *g1, int n1) {

	register int *g1_ptr, *g_ptr;
	int *g1_last, *g_last, *first, *last;
	register int *row_ptr = a->rowptr, *k_ptr = a->k, j;

	g_ptr = g;
	g_last = g + n;

	g1_ptr = g1;
	g1_last = g1 + n1;
	j = 0;

	while(g1_ptr < g1_last && g_ptr < g_last) {

		if(*g1_ptr < *g_ptr) {
			g1_ptr++;
		}

		else {
			if(*g_ptr < *g1_ptr) {
				g_ptr++;
				row_ptr++;
				k_ptr++;
			}

			else {
				first = a->colind + (*row_ptr);
				last = a->colind + *(row_ptr + 1);
				A_add_row_g(sub_mat, first, last, g1, j);
				row_ptr++; g_ptr++; g1_ptr++; k_ptr++; j++;
			}
		}
	}
}


/*
 * builds a sub sparse matrix from a bigger sparse matrix
 */
A* A_split(A* a, int *g, int n, int *g1, int n1){

	A* sub_mat;
	int *k_a = a->k, *k_sub, k;
	int *g_last, *g1_last, *first, *last;
	register int nnz = 0, i;
	register int *sub_k_ptr, *g_ptr, *g1_ptr, *row_ptr;


	k_sub = (int*)malloc(n1 * sizeof(int));
	sub_k_ptr = k_sub;
	row_ptr = a->rowptr;

	g_ptr = g;
	g_last = g + n;

	i = 0;

	g1_ptr = g1;
	g1_last = g1 + n1;

	while(g1_ptr < g1_last && g_ptr < g_last) {

		if(*g1_ptr < *g_ptr) {
			g1_ptr++;
		}

		else {
			if(*g_ptr < *g1_ptr) {
				g_ptr++;
				row_ptr++;
				i++;
			}
			else {
				k = k_a[i];
				first = a->colind + (*row_ptr);
				last = a->colind + *(row_ptr + 1);
				nnz += num_of_shared_values(g1, g1 + n1, first, last);
				*sub_k_ptr = k;
				sub_k_ptr++; g_ptr++; g1_ptr++; row_ptr++; i++;
			}
		}
	}

	sub_mat = A_create(n1, nnz);
	sub_mat->k = k_sub;

	sub_mat->ind_array = (int*)malloc(nnz * sizeof(int));
	exit_if_no_memory(sub_mat->ind_array);

	A_initialize_rows_of_sub_mat(a, g, n, sub_mat, g1, n1);
	sub_mat->m = a->m;
	sub_mat->nnz = nnz;

	return sub_mat;

}


/*
 * Add a row to a sparse matrix
 */
void A_add_row(A* a, const int *row, int size, int i){

	int j;
	register int col, *ptr_1;
	register const int *ptr = row;

	j = a->first_empty;
	(a->rowptr)[i] = j;
	ptr_1 = a->colind + j;

	for(col = 0; col < size; col++){

		*ptr_1++ = *ptr;
		a->first_empty++;
		ptr++;

	}
}

/*
 * fills a row of a sub sparse matrix
 */
void A_add_row_g(A *a_g, int *i_first,
				 int *i_last, int *g, int i) {

	register const int *i_ptr = i_first;
	register int *g_ptr = g, *ptr_col;
	int *g_last = g + a_g->n, j;

	j = a_g->first_empty;
	(a_g->rowptr)[i] = j;
	ptr_col = a_g->colind + j;

	while(i_ptr < i_last && g_ptr < g_last) {

		if(*i_ptr < *g_ptr)
			i_ptr++;

		else {

			if(*g_ptr < *i_ptr)
				g_ptr++;

			else {
				*ptr_col = *g_ptr;
				a_g->first_empty++;
				ptr_col++; i_ptr++; g_ptr++;
			}
		}
	}
}


/*
 * destroys a sparse matrix
 */
void A_destroy(A *a){

	if(a != NULL){
		free(a->colind);
		free(a->rowptr);
		free(a->k);

		if(a->m != a->nnz)
			free(a->ind_array);
	}

	free(a);
}


/*
 * perform a multiplication of a sparse matrix by a vector
 */
void A_multiply(A* a, double *v, double *res, int *g, int n_g, a_mode m) {

	if(a->touched == false) {
		A_slow_multiply(a, v, res, g, n_g, m);
		a->touched = true;
	}

	else
		A_fast_multiply(a, v, res, n_g, m);
}

/*
 * performs a slow multiply of a sub sparse matrix by a vector
 * and collects data for future fast multiplication
 *
 * This function is performed one time only
 */
void A_slow_multiply(A* a_g, double *v, double *res, int *g, int n_g, a_mode m){

	register int *row_ptr, *i_ptr, i, g_id;
	int *i_ptr_last, *g_ptr_last, *colind, *ind_array;
	register int *g_ptr = g, *ptr, *ind_ptr;
	register double *v_ptr, *res_ptr;

	v_ptr = v;
	res_ptr = res;
	colind = a_g->colind;
	g_ptr_last = g + n_g;
	row_ptr = (a_g->rowptr);
	ptr = g;
	ind_ptr = a_g->ind_array;

	if(a_g->m == a_g->nnz)
		ind_array = a_g->colind;
	else
		ind_array = a_g->ind_array;

	for(i = 0; i < n_g; i++){

		i_ptr = colind + *row_ptr;
		ind_ptr = ind_array + *row_ptr;
		i_ptr_last = colind + *(row_ptr + 1);
		g_ptr = g;
		v_ptr = v;
		g_id = 0;

		while(i_ptr < i_ptr_last && g_ptr < g_ptr_last) {

			if(*i_ptr < *g_ptr){
				i_ptr++;
			}
			else if (*g_ptr < *i_ptr) {
				v_ptr++; g_ptr++; g_id++;
			}
			else {
				if(m == f)
					*res_ptr += 1;
				else
					*res_ptr += *v_ptr;
				/* collecting relative indices*/
				*ind_ptr = g_id;
				ind_ptr++; v_ptr++; g_ptr++; i_ptr++; g_id++;
			}
		}

		res_ptr++; ptr++; row_ptr++;
	}

	a_g->touched = true;
}


/*
 * performs a fast multiplication of a sparse matrix by a given vector
 */
void A_fast_multiply(A* a_g, double *v, double *result, int n_g, a_mode m) {

	register int *ind_ptr, *row_ptr, i;
	int *ind_array, *ind_last;
	register double *res_ptr;

	res_ptr = result;
	row_ptr = a_g->rowptr;
	ind_array = a_g->ind_array;

	if(a_g->m == a_g->nnz)
		ind_array = a_g->colind;

	for(i = 0; i < n_g; i++) {

		ind_ptr = ind_array + *row_ptr;
		ind_last = ind_array + *(row_ptr + 1);

		while(ind_ptr < ind_last) {
			if(m == f)
				*res_ptr += 1;
			else
				*res_ptr += v[*ind_ptr];
			ind_ptr++;

		}

		row_ptr++; res_ptr++;
	}
}


