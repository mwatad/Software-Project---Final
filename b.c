/*
 * b.c
 *
 *  Created on: Aug 14, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "error.h"
#include "division.h"
#include "config.h"


/*
 * creating a b hat matrix
 */
B* B_create(A* a, int *g, int n_g, double C) {

	B* b;

	b = (B*)malloc(sizeof(B));
	exit_if_no_memory(b);

	if(g != NULL)
		b->g = g;

	else {
		b->g = (int*)malloc(sizeof(int) * n_g);
		exit_if_no_memory(b->g);
	}

	b->f = (double*)calloc(n_g, sizeof(double));
	exit_if_no_memory(b->f);

	b->a = a;
	b->n_g = n_g;
	b->C = C;

	return b;

}

/*
 * destroying a b hat matrix
 */
void B_destroy(B* b){

	if(b == NULL)
		return;

	free(b->g);
	free(b->f);
	A_destroy(b->a);
	free(b);

}

/*
 * initializing a b hat matrix
 */
void B_initialize_g(B* b) {

	int n_g = b->n_g;
	register int *g_ptr = b->g, i;

	for(i = 0; i < n_g; i++) {
		*g_ptr = i;
		g_ptr++;
	}

	B_calculate_f(b);
}


int B_get_ng(B *b) {

	return b->n_g;
}


/*
 * calculating f vector
 *
 * This function is called one time only for each b hat matrix.
 */
void B_calculate_f(B *b) {

	B_multiply(b, NULL, b->f, b_f);
}


/*
 * calculating 1-norm of a b hat matrix
 */
void B_calculate_1_norm(B *b) {

	register int *i_ptr, *g_ptr, *k_ptr_i, *k_ptr_j, *row_ptr, i;
	int *g = b->g, n_g = b->n_g;
	double M = b->a->m, max = 0, val;
	int *g_last = g + n_g;
	int *i_last, *colind;
	int k_i;

	A *a = b->a;
	k_ptr_i = a->k;
	row_ptr = a->rowptr;
	colind = a->colind;

	for(i = 0; i < n_g; i++) {

		k_i = *k_ptr_i;
		i_ptr = colind + (*row_ptr);
		i_last = colind + *(row_ptr + 1);

		g_ptr = g;
		val = 0;
		k_ptr_j = a->k;

		while(i_ptr < i_last) {

			if(*i_ptr > *g_ptr) {
				val += (k_i * (*k_ptr_j));
				k_ptr_j++; g_ptr++;
			}

			else { /* *i_ptr == *g_ptr */
				val += fabs(M - (k_i * (*k_ptr_j)));
				k_ptr_j++; i_ptr++; g_ptr++;
			}
		}

		while (g_ptr < g_last) {
			val += (k_i * (*k_ptr_j));
			k_ptr_j++; g_ptr++;
		}

		val /= M;
		max = val > max ? val : max;
		row_ptr++; k_ptr_i++;
	}

	b->C = max;
}


/*
 * performs a multiplication of a b hat matrix by a given vector
 */
double B_multiply(B *b, double *v, double *result, b_mode q) {

	int n_g = b->n_g, *g = b->g;
	register int i, *k_ptr;
	register double *f_ptr, k_v = 0, v_norm = 0;
	double m = b->a->m, C = b->C, temp;
	register double *v_ptr, *res_ptr;

	exit_if_dividing_by_zero(m);

	if(q == b_f)
		A_multiply(b->a, NULL, result, g, n_g, f);

	else{
		memset(result, 0, b->a->n * sizeof(double));
		A_multiply(b->a, v, result, g, n_g, mult);
	}

	k_ptr = b->a->k;
	v_ptr = v;

	for(i = 0; i < n_g; i++) {
		if(q == b_f)
			k_v += *k_ptr;
		else
			k_v += (*k_ptr) * (*v_ptr);
		v_ptr++; k_ptr++;
	}

	f_ptr = b->f;
	k_ptr = b->a->k;
	v_ptr = v;
	res_ptr = result;

	for(i = 0; i < n_g; i++) {

		*res_ptr += -((*k_ptr) * k_v) / m;

		if(q == hat || q == shifted)
			*res_ptr += - (*f_ptr) * (*v_ptr);

		if(q == shifted)
			(*res_ptr) += C * (*v_ptr);

		temp = *res_ptr;
		v_norm += temp * temp;
		v_ptr++; res_ptr++; k_ptr++; f_ptr++;
	}

	return sqrt(v_norm);
}



/*
 * calculates B * v_prev and normalizes the result
 */
boolean B_calculate_next(B *b, double *v_prev, double *v_next) {

	int n_g = b->n_g;
	double norm = 0, norm_inverse;
	register int i;
	boolean res = true;
	register double *prev_ptr, *next_ptr;

	norm = B_multiply(b, v_prev, v_next, shifted);
	exit_if_dividing_by_zero(norm);

	norm_inverse = 1 / norm;
	prev_ptr = v_prev;
	next_ptr = v_next;

	for(i = 0; i < n_g; i++){

		(*next_ptr) *= norm_inverse;

		if(res && fabs((*next_ptr) - (*prev_ptr)) >= EPSILON)
			res = false;

		prev_ptr++;
		next_ptr++;
	}

	return res;
}


/*
 * create a random vector
 */
void create_random_vector(double *init_vector, int n_g) {

	register int i;
	register double *ptr = init_vector;

	for(i = 0; i < n_g; i++) {
		*ptr = rand();
		ptr++;
	}
}


/*
 * calculates 2 dot product operations
 *
 * numerator <- v_1 * v_2
 * denominator <- v_1 * v_1
 *
 */
void calculate_dot_product(double *v_1, double *v_2,
		double* numerator, double *denominator, int n_g) {

	register int i;
	register double dot_product_n = 0, dot_product_d = 0;
	register double *ptr_1, *ptr_2;
	double temp;

	ptr_1 = v_1;
	ptr_2 = v_2;

	for(i = 0; i < n_g; i++) {
		temp = *ptr_1;
		dot_product_n += temp * (*ptr_2);
		dot_product_d += temp * temp;
		ptr_1++;
		ptr_2++;
	}

	*numerator = dot_product_n;
	*denominator = dot_product_d;
}


/*
 * finds the leading eigen pair for a b hat matrix
 */
double B_find_leading_eigenpair(B *b, double *leading_v, double *aux_array) {

	double *v_prev, *v_next, *temp;
	double numerator, denominator;
	int cnt = 0;

	int n_g = b->n_g;
	int limit = PI_LIMIT(n_g);

	create_random_vector(aux_array, n_g);

	v_prev = aux_array;
	v_next = leading_v;

	while(true){

		if(cnt == limit)
			exit_infinite_loop_PI();

		if(!B_calculate_next(b, v_prev, v_next)){
			temp = v_prev;
			v_prev = v_next;
			v_next = temp;
			cnt++;
		}

		else
			break;
	}

	B_multiply(b, leading_v, aux_array, shifted);

	calculate_dot_product(leading_v, aux_array, &numerator, &denominator, n_g);
	exit_if_dividing_by_zero(denominator);

	return numerator / denominator - b->C;
}


/*
 * returns the number of positives entries in an array
 */
int num_of_positive_entries(double *leading_v, int n_g) {

	register int i, cnt = 0;
	register double *ptr = leading_v;

	for(i = 0; i < n_g; i++) {
		if(IS_POSITIVE(*ptr))
			cnt++;
		ptr++;
	}

	return cnt;
}


/*
 * divides a group represented by a b hat matrix into two groups
 * according to algorithm 2
 */
void B_divide_into_two_groups(B *b, double *leading_v, double *aux_array) {

	int n_g = b->n_g;
	register int i;
	double modularity;
	double eigen_val;
	register double *leading_ptr;
	register int cnt = 0;

	eigen_val = B_find_leading_eigenpair(b, leading_v, aux_array);

	leading_ptr = leading_v;

	for(i = 0; i < n_g; i++){

		if(IS_POSITIVE(*leading_ptr)) {
			*leading_ptr = 1;
			cnt++;
		}
		else
			*leading_ptr = -1;

		leading_ptr++;
	}

	modularity = B_calculate_modularity(b, leading_v, aux_array);

	if(!IS_POSITIVE(eigen_val) || !IS_POSITIVE(modularity)) {

		leading_ptr = leading_v;

		for(i = 0; i < n_g; i++) {
			*leading_ptr = 1;
			leading_ptr++;
		}
	}
}

/*
 * calculates sBs
 */
double B_calculate_modularity(B *b, double *s, double *res) {

	int n_g;
	register int i;
	register double sum = 0;
	register double *s_ptr, *res_ptr;

	B_multiply(b, s, res, hat);

	n_g = b->n_g;
	res_ptr = res;
	s_ptr = s;

	for(i = 0; i < n_g; i++) {
		sum += (*s_ptr) * (*res_ptr);
		s_ptr++;
		res_ptr++;
	}

	return sum;
}

/*
 * updates b1 and b2 based on the division that is defined by s
 */
void B_update_groups(B **b, B **b1, B **b2, double *s) {

	int n_g1, n_g, *ptr_1, *ptr_2, g_i, *g1, *g2;
	register int i, *g_ptr = (*b)->g;
	register double *s_ptr;

	n_g = (*b)->n_g;
	n_g1 = num_of_positive_entries(s, n_g);

	if(n_g1 == 0 || n_g1 == n_g) {

		*b1 = *b;
		*b = NULL;
		*b2 = B_create(NULL, NULL, 0, 0);
		return;
	}

	g1 = (int*)malloc(n_g1 * sizeof(int));
	exit_if_no_memory(g1);
	*b1 = B_create(NULL, g1, n_g1, (*b)->C);

	g2 = (int*)malloc((n_g - n_g1) * sizeof(int));
	exit_if_no_memory(g2);
	*b2 = B_create(NULL, g2, n_g - n_g1, (*b)->C);

	ptr_1 = (*b1)->g;
	ptr_2 = (*b2)->g;

	s_ptr = s;

	for(i = 0; i < n_g; i++) {

		g_i = *g_ptr;

		if(*s_ptr == 1) {
			*ptr_1 = g_i;
			ptr_1++;
		}
		else {
			*ptr_2 = g_i;
			ptr_2++;
		}

		s_ptr++; g_ptr++;
	}

	(*b1)->a = A_split((*b)->a, (*b)->g, (*b)->n_g, (*b1)->g, (*b1)->n_g);
	B_calculate_f((*b1));

	(*b2)->a = A_split((*b)->a, (*b)->g, (*b)->n_g, (*b2)->g, (*b2)->n_g);
	B_calculate_f((*b2));
}



/*
 * updates the vector b*s after negating s[j_tag]
 */
void update_vals(B *b, double *vals, double *s, int j_tag) {

	int *row_arr, *i_last, *g_last, *k_array, *g, k_j_tag, s_j_tag;
	register int *j_ptr, *g_ptr, *k_ptr;
	register double *vals_ptr;
	double m;
	A *a;

	a = b->a;
	m = b->a->m;
	row_arr = a->rowptr;
	k_array = a->k;
	k_j_tag = k_array[j_tag];
	s_j_tag = s[j_tag];

	g = b->g;
	g_ptr = g;
	g_last = g + b->n_g;

	j_ptr = b->a->colind + row_arr[j_tag];
	i_last = b->a->colind + row_arr[j_tag + 1];;

	k_ptr = b->a->k;
	vals_ptr = vals;

	while(j_ptr < i_last && g_ptr < g_last) {

		if(*j_ptr < *g_ptr)
			j_ptr++;

		else {

			if(*g_ptr < *j_ptr) {
				*vals_ptr += -2 * s_j_tag * k_j_tag * (*k_ptr) / m;
				vals_ptr++; k_ptr++; g_ptr++;
			}

			else {
				*vals_ptr += 2 * s_j_tag * (1 - k_j_tag * (*k_ptr) / m);
				vals_ptr++; k_ptr++; g_ptr++; j_ptr++;
			}
		}
	}

	while(g_ptr < g_last) {
		*vals_ptr += -2 * s_j_tag * k_j_tag * (*k_ptr) / m;
		vals_ptr++; k_ptr++; g_ptr++;
	}
}

/*
 * calculates score[k] based on vals[k]
 */
double calculate_score(double M, double s_k, double val, int rank) {

	return (-4 * (M * (s_k) * val + rank * rank)) / M;
}

/*
 * calculating max score (for algorithm 4)
 */
void find_max_score(B *b, double *max_score_out, int *j_tag_out,
		double *s, double *vals, status *unmoved_arr) {

	int k, n_g = b->n_g, rank, j_tag;
	double score, M = b->a->m, max_score;
	boolean init_max_score = false;


	register double *s_ptr = s;
	register double *vals_ptr = vals;
	register int *k_ptr = b->a->k;
	register status *um_ptr = unmoved_arr;


	for(k = 0; k < n_g; k++) {

		if(*um_ptr == unmoved){

			rank = *k_ptr;
			score = calculate_score(M, *s_ptr, *vals_ptr, rank);

			if(!init_max_score) {
				max_score = score;
				j_tag = k;
				init_max_score = true;
			}
			else {
				if(GREATER_THAN(score, max_score)) {
					max_score = score;
					j_tag = k;
				}
			}
		}

		s_ptr++; vals_ptr++; k_ptr++; um_ptr++;
	}

	*j_tag_out = j_tag;
	*max_score_out = max_score;

}

/*
 * updating improvement (algorithm 4)
 */
void update_improvement(int i, double max_score,
		double *imp, double *max_improve, int *i_tag) {

	if(i == 0) {
		*imp = max_score;
		*max_improve = max_score;
		*i_tag = 0;
	}

	else {
		*imp += max_score;
		if(GREATER_THAN(*imp, *max_improve)) {
			*max_improve = *imp;
			*i_tag = i;
		}
	}

}

/*
 * implements algorithm 4 (maximizes a given division)
 */
void B_maximize_modularity(B **b, B **b1, B **b2, double *s, double *temp,
		double *aux_arr,int *indices, status *unmoved_arr) {

	int n_g, j_tag = -1, i_tag;
	register int *ind_ptr, i, j;
	double max_score, delta_Q, max_improve, imp;
	double mod_before, mod_after;

	delta_Q = 0;
	n_g = (*b)->n_g;

	memcpy(temp, s, n_g * sizeof(double));
	mod_before = B_calculate_modularity(*b, s, aux_arr);
	B_multiply(*b, s, aux_arr, normal);

	do {

		memset(unmoved_arr, 0, n_g * sizeof(status));
		ind_ptr = indices;

		for(i = 0; i < n_g; i++) {

			find_max_score(*b, &max_score, &j_tag, s, aux_arr, unmoved_arr);
			s[j_tag] *= -1;
			*ind_ptr = j_tag;
			update_vals(*b, aux_arr, s, j_tag);
			update_improvement(i, max_score, &imp, &max_improve, &i_tag);
			unmoved_arr[j_tag] = moved;
			ind_ptr++;
		}

		ind_ptr = indices + n_g - 1;

		for(i = n_g - 1; i > i_tag; i--) {
			j = *ind_ptr;
			s[j] *= -1;
			update_vals(*b, aux_arr, s, j);
			ind_ptr--;
		}

		if(i_tag == n_g - 1)
			delta_Q = 0;
		else
			delta_Q = max_improve;
	}

	while(IS_POSITIVE(delta_Q));
	mod_after = B_calculate_modularity(*b, s, aux_arr);

	if(GREATER_THAN(mod_after, mod_before))
		B_update_groups(b, b1, b2, s);
	else
		B_update_groups(b, b1, b2, temp);

}

