#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/*
This approach is good since it provides extensibility for more operations of aggregate function and it also provides a solution for non-associative operations like the mean and geometric mean.
For each element, we perform only one operation on the int/floating-point numbers (addition) and for the final result we just perform one operation (division).

Non-assoicative operation can be converted to some another associative function with an additional final operation special for the non-associative operation and applied on the output of the intermediate stage.
We can divide the operation into 2 stages.
	1. Intermediate stage which applies the associative operation on the n elements.
	2. Final stage which applies the output function on the result of the previous stage.
*/

void* aggregate(void* base, size_t size, int n, void* initial_value, void* (*opr)(const void*, const void*));

// Applies the addition operation on two elements
void* addInt(const void* a, const void* b){
	int* output = malloc(sizeof(int));
	*output = (*((int*)a) + *((int*)b));
	return output;
}

// Applies the addition operation on two elements
void* addDouble(const void* a, const void* b){
        double* output = malloc(sizeof(double)); 
        *output = (*((double*)a) + *((double*)b));
        return output;
}

// Applies the multiplication operation on two elements
void* mulInt(const void* a, const void* b){
	int* output = malloc(sizeof(int));
	*output = (*((int*)a) * *((int*)b));
	return output;
}

// Applies the multiplication operation on two elements
void* mulDouble(const void* a, const void* b){
        double* output = malloc(sizeof(double));
        *output = (*((double*)a) * *((double*)b));
        return output;
}
// Applies the arithmetic mean operation on two elements
void* meanInt(const void* a, const void* b){
	double* output = malloc(sizeof(double));
	*output = (*((int*)a) + *((int*)b))/2.0;
	return output;
}

// Applies the arithmetic mean operation on two elements
void* meanDouble(const void* a, const void* b){
        double* output = malloc(sizeof(double));
        *output = (*((double*)a) + *((double*)b))/2.0;
        return output;
}


// Applies the output function sum/n for mean ints
void* intMeanOutputFunc(const void* a, const void* b){
	int* n = (int*) b;
	int* result = (int*) a;
	
	double* output = malloc(sizeof(double));
	*output = (double)*result / (double)*n;
	
	return output;
}

// Applies the output function sum/n for mean doubles
void* doubleMeanOutputFunc(const void* a, const void* b){
	int* n = (int*) b;
	double* result = (double*) a;
	*result = *result / *n;
	
	return result;
}


// provides aggregation capability for nonassociative operations like mean and geometric mean
// It accepts the associative function and the output function
void* nonassociativeAggregate(void* base, size_t size, int n, void* initial_value, void* (*associative_agg_fun) (const void*, const void*), void* (*output_func)(const void*, const void*), void* output_func_param){

	if (n<2 || base==NULL){
		return base;
	}
	
	void* result = aggregate(base, size, n, initial_value, associative_agg_fun);

	if (result == NULL) return NULL;
	
	result = output_func(result, output_func_param);

	return result;
}


// checks the special conditions for aggregate functions
void* checkSpecialConds(void* base, size_t size, int n, void* initial_value, void* (*opr)(const void*, const void*)){

	if (opr==meanInt){
		return nonassociativeAggregate(base, size, n, initial_value, &addInt, &intMeanOutputFunc, &n);
	}else if (opr==meanDouble){
		return nonassociativeAggregate(base, size, n, initial_value, &addDouble, &doubleMeanOutputFunc, &n);
	}
	
	return NULL;
}

// Applies the aggregation operation opr on n elements
void* aggregate(void* base, size_t size, int n, void* initial_value, void* (*opr)(const void*, const void*)){

	if (base==NULL || initial_value==NULL || opr==NULL){
		return NULL;
	}
	
	if (n<2){
		return base;
	}

	void* output = checkSpecialConds(base, size, n, initial_value, opr);
	if (output) return output;

	output = initial_value;

	for (int i=0; i<n; i++){
		output = opr(output, base + size * i);
	}

	return output;
}

// Main function
int main(){
        int n = 5;
        int* arr = calloc(n, sizeof(int));

        printf("ints = [");
        arr[0] = 8;
	printf("%d,", arr[0]);
        arr[1] = 13;
	printf("%d,", arr[1]);
        for (int i=2; i<n; i++){
                *(arr+i) = *(arr+i-1) + *(arr+i-2);

                printf("%d,", arr[i]);
        }
        printf("\b]\n");

        int n2 = 5;
        double* arr2 = calloc(n2, sizeof(double));

        printf("doubles = [");
        arr2[0] = 1;
        printf("%.2f,", arr2[0]);
        arr2[1] = 2;
        printf("%.2f,", arr2[1]);
        for (int i=2; i<n2; i++){
                *(arr2+i) = *(arr2+i-1) + *(arr2+i-2);

                printf("%.2f,", arr2[i]);
        }
        printf("\b]\n");


	int* init1 = malloc(sizeof(int));
	*init1 = 0;
	int* init2 = malloc(sizeof(int));
	*init2 = 1;
	double* init3= malloc(sizeof(double));
	*init3 = 0.0;
	double* init4 = malloc(sizeof(double));
	*init4 = 1.0;


	// Addition
        int* result1a = (int*)aggregate(arr, sizeof(int), n, init1, addInt);
        printf("Addition(ints) = %d\n", *result1a);

	double* result2a = (double*)aggregate(arr2, sizeof(double), n2, init3, addDouble);
        printf("Addition(doubles) = %.2f\n", *result2a);

	// Multiplication
        int* result1m = (int*)aggregate(arr, sizeof(int), n, init2, mulInt);
        printf("Multiplication(ints) = %d\n", *result1m);

	double* result2m = (double*)aggregate(arr2, sizeof(double), n2, init4, mulDouble);
        printf("Multiplication(doubles) = %.2f\n", *result2m);

	// Arithmetic Mean
        double* result1mean = (double*)aggregate(arr, sizeof(int), n, init1, meanInt);
        printf("Mean(ints) = %.2f\n", *result1mean);

	double* result2mean = (double*)aggregate(arr2, sizeof(double), n2, init3, meanDouble);
        printf("Mean(doubles) = %.2f\n", *result2mean);



        // Free pointers
        free(arr);free(arr2);free(init1);free(init4);free(init2);free(init3);
        free(result1a);free(result2a);free(result1m);free(result2m);
        free(result1mean);free(result2mean);


        return EXIT_SUCCESS;
}


