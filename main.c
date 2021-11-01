/*
 * main.c
 *
 *  Created on: Aug 14, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */


#include <stdlib.h>
#include <time.h>
#include "error.h"
#include "parser.h"
#include "SPBufferset.h"

/*
 * Auxiliary functions
 *
 */
void initialize_vectors(double **leading_v, double **temp, double **aux_array,
					int **indices, status **unmoved, int n);

void free_arrays(double *leading_v, double *temp, double *aux_array,
					int *indices, status *unmoved);

void update_trivial_partition(Division *O, B *b1, B *b2);

void update_non_trivial_partition(Division *P, Division *O, B *b1, B *b2);

/********************************************************************************/





/*
 *
 * main program - implementing clustering algorithm
 *
 */
int main(int argc, char **argv) {

	B *b, *b1, *b2;
	int n, *indices;
	double *leading_v, *aux_array, *temp;
	Division *P, *O;
	clock_t start, end;
	status *unmoved;

	start = clock();

	if(argc != 3)
		exit_insufficient_arguments();

	SP_BUFF_SET();
	srand(time(NULL));

	b = parser_read_input(argv[1]);
	n = b->a->n;

	P = division_create();
	O = division_create();

	initialize_vectors(&leading_v, &temp, &aux_array, &indices, &unmoved, n);

	B_calculate_1_norm(b);
	division_add_group(P, b);

	while(division_get_size(P) != 0) {

		b = division_extract_group(P);
		B_divide_into_two_groups(b, leading_v, aux_array);
		B_maximize_modularity(&b, &b1, &b2, leading_v, aux_array, temp, indices, unmoved);
		B_destroy(b);

		if(B_get_ng(b1) == 0 || B_get_ng(b2) == 0)
			update_trivial_partition(O, b1, b2);

		else
			update_non_trivial_partition(P, O, b1, b2);
	}

	parser_write_output(argv[2], O);

	/*
	division_print(O);
	*/

	free_arrays(leading_v, temp, aux_array, indices, unmoved);
	division_destroy(P);
	division_destroy(O);

	end = clock();
	printf("Program took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}


/*
 * allocates few vectors for future use
 */
void initialize_vectors(double **leading_v, double **temp, double **aux_array,
	int **indices, status **unmoved, int n) {

	*leading_v = (double*)malloc(sizeof(double) * n);
	exit_if_no_memory(*leading_v);

	*temp = (double*)malloc(sizeof(double) * n);
	exit_if_no_memory(*temp);

	*aux_array = (double*)calloc(n, sizeof(double));
	exit_if_no_memory(*aux_array);

	*indices = (int*)calloc(n, sizeof(int));
	exit_if_no_memory(*indices);

	*unmoved = (status*)malloc(sizeof(status) * n);
	exit_if_no_memory(*unmoved);

}


/*
 * deallocates vectors
 *
 */
void free_arrays(double *leading_v, double *temp, double *aux_array,
					int *indices, status *unmoved) {

	free(leading_v);
	free(temp);
	free(aux_array);
	free(indices);
	free(unmoved);
}


/*
 * updating P and O when facing a trivial partition
 */
void update_trivial_partition(Division *O, B *b1, B *b2) {

	if(B_get_ng(b1) == 0) {
		B_destroy(b1);
		division_add_group(O, b2);
	}
	else {
		B_destroy(b2);
		division_add_group(O, b1);
	}
}


/*
 * updating P and O when facing a non trivial partition
 */
void update_non_trivial_partition(Division *P, Division *O, B *b1, B *b2) {

	if(B_get_ng(b1) == 1)
		division_add_group(O, b1);
	else
		division_add_group(P, b1);

	if(B_get_ng(b2) == 1)
		division_add_group(O, b2);
	else
		division_add_group(P, b2);
}
