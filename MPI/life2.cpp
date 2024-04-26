// #define ENABLE_HDF

#ifdef ENABLE_HDF
    #include "hdf5.h"
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"  

#define N               30          // matrix length
#define T               8000        // number of timesteps
#define P               65          // probability of alive initial

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* 
    setting up the memory for all timestep's matrix and initialise the first timestep's matrix 
*/
int* setup(int matrixLength, int timeSteps, int probAlive) {
    int matrixSize = matrixLength * matrixLength;
    
    // initialise the seed of the random number
    srand(time(NULL));

    // set of the memory for all matrix
    int *matrixALL = (int*)malloc(matrixSize * timeSteps * sizeof(int));

    // initialise matrix
    for (int i = 0; i < matrixSize; i++)
    {
        matrixALL[i] = (rand() % 100) < probAlive;
        if (i % matrixLength == 0 
            || i % matrixLength == (matrixLength-1)
            || i < matrixLength
            || i > matrixLength * (matrixLength-1)) {
            matrixALL[i] = 0;
        }
    }
    return matrixALL;
}


/*
    printing out the given timestep from matrixAll
*/
void printTimeSeries(int* matrixALL, int timeCount, int matrixLength) 
{
    int matrixSize = matrixLength * matrixLength;
    int step = 0;

    for(int i = 0; i < matrixSize * timeCount; i++) {
        if(i % matrixSize == 0) {
            printf("\n\nTime step %d", step);
            step++;
        }

        if(i % matrixLength == 0) {
            printf("\n");
        }
        printf("%d ", matrixALL[i]);
    }
    printf("\n\n");
}


/*
    count the number of eighbour that is alive at given timestep
*/
inline int countNeighbour(int* matrixALL, int matrixLength, int r, int c) 
{
    int count = 0;

    // count number of alive cell include itself
    for (int i = r-1; i <= r+1; i++) {
        for (int j = c-1; j <= c+1; j++) {
            count += matrixALL[i * matrixLength + j];
        }
    }
    
    // remove current cell's state
    count -= matrixALL[r * matrixLength + c];

    return count;
}


void goNext(int *matrix, int matrixLength, int start, int end)
{   
    int* matrix_copy = (int*)malloc(sizeof(int) * matrixLength * matrixLength);

    for (int i = 0; i < (matrixLength); i++) {
        for (int j = 0; j < (matrixLength); j++) {
            matrix_copy[i * matrixLength + j] = matrix[i * matrixLength + j];
        }
    }

    for (int i = (start+1); i < (end); i++) {
        for (int j = 1; j < (matrixLength-1); j++) {
            int neighbour_live_cell = countNeighbour(matrix_copy, matrixLength, i, j);

            if ((matrix_copy[i * matrixLength + j] == 1)
                && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
                matrix[i * matrixLength + j] = 1;
            } else if ((matrix_copy[i * matrixLength + j] == 0)
                && (neighbour_live_cell == 3)) {
                matrix[i * matrixLength + j] = 1;
            } else {
                matrix[i * matrixLength + j] = 0;
            }
        }
    }

    free(matrix_copy);
}


/*
    saving data using .h5 file for visualise
*/
#ifdef ENABLE_HDF
    int mat2hdf5 (int* wdata, int timeCount, int particleCount, const char* FILE, const char* DATASET) 
    {
        // create file, dataspace and dataset
        hid_t file = H5Fcreate (FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        hsize_t dims[2] = {(unsigned long)timeCount, (unsigned long)particleCount};
        hid_t space = H5Screate_simple (2, dims, NULL);
        hid_t dset = H5Dcreate (file, DATASET, H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        //write the floating point data to dataset. 
        herr_t status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);

        // close and release resources.
        status = H5Dclose (dset);
        status = H5Sclose (space);
        status = H5Fclose (file);

        return 0;
    }
#endif


int main (int argc, char *argv[]) 
{   
    int processId, processCount;
    MPI_Init(&argc, &argv);  
    MPI_Status status;  
    MPI_Request require;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);  
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);  

    // adding "walls" into the matrix length
    int totalLength = N+2;
    int begin_Arow, end_Arow;
    int avg_rows = N / (processCount - 1);
    int* matrixAll;
    int* local_A;

    if (processId == 0) {
        // initialise the matrix
        matrixAll = setup(totalLength, T, P);

        // getting and saving each timestep
        for (int t = 0; t < (T-1); t++) {
            
            for (int i = 0; i < processCount - 1; i++) {
                begin_Arow = avg_rows * i;
                end_Arow = (i+1 == processCount-1) ? MAX(N, avg_rows * (i+1)) : avg_rows * (i+1);
                
                MPI_Send(&end_Arow, 1, MPI_INT, i+1, 10, MPI_COMM_WORLD); 
                MPI_Send(&matrixAll[t * totalLength * totalLength], 
                            totalLength * totalLength, MPI_INT, i+1, 2, MPI_COMM_WORLD);
            }

            for (int i = 0; i < processCount - 1; i++) {
                begin_Arow = avg_rows * i;
                end_Arow = (i+1 == processCount-1) ? MAX(N, avg_rows * (i+1)) : avg_rows * (i+1);
                
                MPI_Recv(&matrixAll[(t+1) * totalLength * totalLength + ((begin_Arow+1) * totalLength)], 
                            (end_Arow - begin_Arow) * totalLength, MPI_INT, i+1, 3+i, MPI_COMM_WORLD, &status);
            }
        }


        // printing or saving data
        #ifdef ENABLE_HDF
            mat2hdf5(matrixAll, T, (totalLength * totalLength), "life.h5", "DS1");
        #else
            // printTimeSeries(matrixAll, T, totalLength);
        #endif

        free(matrixAll);
    } else {
        
        local_A = (int*)malloc(sizeof(int) * totalLength * totalLength);

        for (int t = 0; t < (T-1); t++) {
            MPI_Recv(&end_Arow, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
            begin_Arow = avg_rows * (processId - 1);
            MPI_Recv(local_A, totalLength * totalLength, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

            goNext(local_A, totalLength, begin_Arow, end_Arow+1);

            MPI_Send(&local_A[(begin_Arow+1) * totalLength], 
                        (end_Arow - begin_Arow) * totalLength, MPI_INT, 0, 3+processId-1, MPI_COMM_WORLD);
        }

        free(local_A);
    }

    MPI_Finalize(); 
}