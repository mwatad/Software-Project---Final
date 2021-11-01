/*
 * parser.h
 *
 *  Created on: Aug 13, 2020
 *  	Author: Muhammad Watad && Shadi Abu Shqara
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "division.h"

/**
 * 	parser_read_input: reads a graph representation from a file
 * 		and writes it to a new created B hat matrix.
 *
 * @param fileName - the name of the file to read from
 *
 * @return
 * 		a new created B hat matrix
 */
B* parser_read_input(char *fileName);

/**
 * 	parser_write_output: writes a division to an output file
 *
 *
 * @param fileName - the name of the output file
 * @param division - a division
 *
 * @return
 *
 */
void parser_write_output(char *fileName, Division *division);


#endif /* PARSER_H_ */
