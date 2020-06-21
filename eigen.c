/*
 * eigen.c
 *
 *  Created on: 7 Apr 2020
 *      Author: avikef
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

double* multiplyMatrixByVector(FILE *matrix, double *vector, int N, double *result_vector);
double* getRowFromMatrix(FILE *matrix, int N);
double multiplyVectorByVector(double *vector1, double *vector2, int N);
double* divideVectorByK(double *vector, double K, int N);
int isLegal(double *vector1, double *vector2, int N);

double epsilon = 0.00001;
int num = 1;

double* multiplyMatrixByVector(FILE *matrix, double *vector, int N, double *result_vector){
	double *row;
	int i;

	for (i = 0; i < N; i++){
		row = getRowFromMatrix(matrix, N);
		*result_vector = multiplyVectorByVector(row, vector, N);
		free(row);
		result_vector++;
	}
	fseek(matrix, sizeof(int)*2, SEEK_SET);
	result_vector -= N;
	return result_vector;
}
double* getRowFromMatrix(FILE *matrix, int N){
	double *row;
	row = malloc(N*sizeof(double));
	fread(row, sizeof(double), N, matrix);
	return row;
}
double multiplyVectorByVector(double *vector1, double *vector2, int N){
	double dot_product;
	int i;

	dot_product = 0.0;
	for (i = 0; i < N; i = i + 1){
		dot_product = dot_product + (double)((*vector1) * (*vector2));
		vector1 = vector1 + 1;
		vector2 = vector2 + 1;
	}
	return dot_product;
}
double* divideVectorByK(double *vector, double K, int N){
	int i;

	for(i = 0; i < N; i++){
		*vector = (double)(*vector / K);
		vector++;
	}
	vector -= N;
	return vector;
}
int isLegal(double *vector1, double *vector2, int N){
	int i;
	double result;

	for(i = 0; i < N; i++){
		result = fabs(*vector1 - *vector2);
		if(result > epsilon){
			return 0;
		}
		vector1++;
		vector2++;
	}
	return 1;
}


int main(int argc, char* argv[]){
	FILE *matrix_file, *output_vector;
	double *result_vector, *ptr_1, *ptr_2, *prev_vector, *temp;
	int rows, i, legal;
	char *matrix_path, *output_path;
	double denominator;
	clock_t start, end;

	start = clock();

	assert(argc == 3);

	legal = 0;
	srand(time(NULL));

	matrix_path = argv[1];
	output_path = argv[2];

	matrix_file = fopen(matrix_path, "rb");
	fread(&rows, sizeof(int), 1, matrix_file);
	fseek(matrix_file, sizeof(int), SEEK_CUR);

	prev_vector = malloc(sizeof(double)*rows);
	result_vector = malloc(sizeof(double)*rows);


	for(i = 0; i < rows; i++){
		*prev_vector = (double)rand();
		prev_vector++;
	}
	prev_vector -= rows;



	while (legal == 0){
		result_vector = multiplyMatrixByVector(matrix_file, prev_vector, rows, result_vector);


		ptr_1 = result_vector;
		ptr_2 = result_vector;
		denominator = sqrt(multiplyVectorByVector(ptr_1, ptr_2, rows));

		result_vector = divideVectorByK(result_vector, denominator, rows);

		legal = isLegal(result_vector, prev_vector, rows);

		temp = prev_vector;
		prev_vector = result_vector;
		result_vector = temp;
	}

	output_vector = fopen(output_path, "wb");
	fwrite(&num, sizeof(int), 1, output_vector);
	fwrite(&rows, sizeof(int), 1, output_vector);
	fwrite(result_vector, sizeof(double), rows, output_vector);

	end = clock();
	printf("%f", (float)(end - start) / CLOCKS_PER_SEC);

	fclose(output_vector);
	fclose(matrix_file);

	free(result_vector);
	free(prev_vector);
	return 1;
}
