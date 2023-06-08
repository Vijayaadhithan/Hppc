#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
typedef struct {
    double** elements;
    int size;
} Matrix;

typedef struct {
    double* elements;
    int size;
} Vector;

typedef struct {
    double eigenvalue;
    Vector eigenvector;
} EigenResult;

EigenResult powerIteration(const Matrix* A, const Vector* x, double epsilon);
EigenResult powerIteration2(const Matrix* A, const Vector* x, double epsilon);
EigenResult powerIteration3(const Matrix* A, const Vector* x, double epsilon);


EigenResult powerIteration(const Matrix* A, const Vector* x, double epsilon) {
    int i, j;
    double* z = (double*)malloc(A->size * sizeof(double));
    double* e = (double*)malloc(A->size * sizeof(double));
    double zmax, emax;
    Vector currentVector = *x;
    EigenResult result;

    do {
        for (i = 0; i < A->size; i++) {
            z[i] = 0.0;
            for (j = 0; j < A->size; j++) {
                z[i] += A->elements[i][j] * currentVector.elements[j];
            }
        }

        zmax = fabs(z[0]);
        for (i = 1; i < A->size; i++) {
            if (fabs(z[i]) > zmax) {
                zmax = fabs(z[i]);
            }
        }

        double invZmax = 1.0 / zmax;
        for (i = 0; i < A->size; i++) {
            z[i] *= invZmax;
        }

        for (i = 0; i < A->size; i++) {
            e[i] = fabs(fabs(z[i]) - fabs(currentVector.elements[i]));
        }

        emax = e[0];
        for (i = 1; i < A->size; i++) {
            if (e[i] > emax) {
                emax = e[i];
            }
        }

        for (i = 0; i < A->size; i++) {
            currentVector.elements[i] = z[i];
        }
    } while (emax > epsilon);

    result.eigenvalue = zmax;
    result.eigenvector = currentVector;

    free(z);
    free(e);

    return result;
}


EigenResult powerIteration2(const Matrix* A, const Vector* x, double epsilon) {
    int i, j;
    double* z = (double*)malloc(A->size * sizeof(double));
    double* e = (double*)malloc(A->size * sizeof(double));
    double zmax, emax;
    Vector currentVector = *x;
    EigenResult result;

    do {
        for (i = 0; i < A->size; i += 2) {
            z[i] = 0.0;
            z[i + 1] = 0.0;
            for (j = 0; j < A->size; j++) {
                z[i] += A->elements[i][j] * currentVector.elements[j];
                z[i + 1] += A->elements[i + 1][j] * currentVector.elements[j];
            }
        }

        zmax = fabs(z[0]);
        for (i = 1; i < A->size; i++) {
            if (fabs(z[i]) > zmax) {
                zmax = fabs(z[i]);
            }
        }

        double invZmax = 1.0 / zmax;
        for (i = 0; i < A->size; i++) {
            z[i] *= invZmax;
        }

        for (i = 0; i < A->size; i += 2) {
            e[i] = fabs(fabs(z[i]) - fabs(currentVector.elements[i]));
            e[i + 1] = fabs(fabs(z[i + 1]) - fabs(currentVector.elements[i + 1]));
        }

        emax = e[0];
        for (i = 1; i < A->size; i++) {
            if (e[i] > emax) {
                emax = e[i];
            }
            }

        for (i = 0; i < A->size; i++) {
            currentVector.elements[i] = z[i];
        }
    } while (emax > epsilon);

    result.eigenvalue = zmax;
    result.eigenvector = currentVector;

    free(z);
    free(e);

    return result;
}

EigenResult powerIteration3(const Matrix* A, const Vector* x, double epsilon) {
    int i, j;
    double* z = (double*)malloc(A->size * sizeof(double));
    double* e = (double*)malloc(A->size * sizeof(double));
    double zmax, emax;
    Vector currentVector = *x;
    EigenResult result;

    do {
        #pragma omp parallel for shared(A, currentVector, z) private(i, j) num_threads(4)
        for (i = 0; i < A->size; i++) {
            z[i] = 0.0;
            for (j = 0; j < A->size; j++) {
                z[i] += A->elements[i][j] * currentVector.elements[j];
            }
        }

        zmax = fabs(z[0]);
        for (i = 1; i < A->size; i++) {
            if (fabs(z[i]) > zmax) {
                zmax = fabs(z[i]);
            }
        }

        double invZmax = 1.0 / zmax;
        #pragma omp parallel for shared(z, invZmax) private(i) num_threads(4)
        for (i = 0; i < A->size; i++) {
            z[i] *= invZmax;
        }

        #pragma omp parallel for shared(z, currentVector, e) private(i) num_threads(4)
        for (i = 0; i < A->size; i++) {
            e[i] = fabs(fabs(z[i]) - fabs(currentVector.elements[i]));
        }

        emax = e[0];
        for (i = 1; i < A->size; i++) {
            if (e[i] > emax) {
                emax = e[i];
            }
        }

        #pragma omp parallel for shared(currentVector, z) private(i) num_threads(4)
        for (i = 0; i < A->size; i++) {
            currentVector.elements[i] = z[i];
        }
    } while (emax > epsilon);

    result.eigenvalue = zmax;
    result.eigenvector = currentVector;

    free(z);
    free(e);

    return result;
}
            
            
            
int main() {
    Matrix A;
    Vector x;
    EigenResult result1, result2, result3 ;
    int i, j;
    clock_t start, end;
    double cpu_time_used1, cpu_time_used2, cpu_time_used3;

    // Read input from file
    FILE* file = fopen("100.txt", "r");
    if (file == NULL) {
        printf("Failed to open input file.\n");
        return 1;
    }

    // Read matrix A size
    fscanf(file, "%d", &A.size);

    // Allocate memory for matrix A
    A.elements = (double**)malloc(A.size * sizeof(double*));
    for (i = 0; i < A.size; i++) {
        A.elements[i] = (double*)malloc(A.size * sizeof(double));
    }

    // Read matrix A elements
    for (i = 0; i < A.size; i++) {
        for (j = 0; j < A.size; j++) {
            fscanf(file, "%lf", &A.elements[i][j]);
        }
    }

    // Read vector x size
    fscanf(file, "%d", &x.size);

    // Allocate memory for vector x
    x.elements = (double*)malloc(x.size * sizeof(double));

    // Read vector x elements
    for (i = 0; i < x.size; i++) {
        fscanf(file, "%lf", &x.elements[i]);
    }

    fclose(file);

    // Set predefined epsilon value
    double epsilon = 0.001;

    start = clock();
    result1 = powerIteration(&A, &x, epsilon);
    end = clock();
    cpu_time_used1 = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Power Iteration:\n");
    printf("Eigenvalue: %.3f\n", result1.eigenvalue);
    printf("Eigenvector: ");
    for (i = 0; i < A.size; i++) {
        printf("%.3f ", result1.eigenvector.elements[i]);
    }
    printf("\n");
    printf("Time taken: %f seconds\n", cpu_time_used1);

    
    start = clock();
    result2 = powerIteration2(&A, &x, epsilon);
    end = clock();
    cpu_time_used2 = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Power Iteration2:\n");
    printf("Eigenvalue: %.3f\n", result2.eigenvalue);
    printf("Eigenvector: ");
    for (i = 0; i < A.size; i++) {
        printf("%.3f ", result2.eigenvector.elements[i]);
    }
    printf("\n");
    printf("Time taken: %f seconds\n", cpu_time_used2);
    
    start = clock();
    result3 = powerIteration3(&A, &x, epsilon);
    end = clock();
    cpu_time_used3 = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Power Iteration3:\n");
    printf("Eigenvalue: %.3f\n", result3.eigenvalue);
    printf("Eigenvector: ");
    for (i = 0; i < A.size; i++) {
        printf("%.3f ", result3.eigenvector.elements[i]);
    }
    printf("\n");
    printf("Time taken: %f seconds\n", cpu_time_used3);

    // Free allocated memory
for (i = 0; i < A.size; i++) {
    free(A.elements[i]);
}
free(A.elements);
free(x.elements);

    return 0;
}