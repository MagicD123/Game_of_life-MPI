// #define ENABLE_HDF

#ifdef ENABLE_HDF
    #include "hdf5.h"
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define N               30          // matrix length
#define T               8000        // number of timesteps
#define P               65          // probability of alive initial


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
}


/*
    count the number of eighbour that is alive at given timestep
*/
inline int countLiveNeighbour(int* matrixALL, int timeCount, int matrixLength, int r, int c) 
{
    int count = 0;
    int currentMatrix = timeCount * matrixLength * matrixLength;

    // count number of alive cell include itself
    for (int i = r-1; i <= r+1; i++) {
        for (int j = c-1; j <= c+1; j++) {
            count += matrixALL[currentMatrix + i * matrixLength + j];
        }
    }
    
    // remove current cell's state
    count -= matrixALL[currentMatrix + r * matrixLength + c];

    return count;
}


/*
    getting and update the matrix for next timestep
*/
void nextStep(int* matrixALL, int timeCount, int matrixLength)
{   
    int neighbour_live_cell, neighbour_live_cell_11, neighbour_live_cell_12, 
            neighbour_live_cell_21, neighbour_live_cell_22;
    int currentMatrix = timeCount * matrixLength * matrixLength;
    int nextMatrix = (timeCount+1) * matrixLength * matrixLength;

    // doing with group of four (2*2 small matrix) cells per step of for loop
    for (int i = 1; i < (matrixLength-2); i+=2) {
        for (int j = 1; j < (matrixLength-2); j+=2) {

            // top-left
            neighbour_live_cell_11 
                = countLiveNeighbour(matrixALL, timeCount, matrixLength, i, j);
            if (matrixALL[currentMatrix + i * matrixLength + j] == 1
                    && (neighbour_live_cell_11 == 2 || neighbour_live_cell_11 == 3)) {
                matrixALL[nextMatrix + i * matrixLength + j] = 1;
            } 
            else if (matrixALL[currentMatrix + i * matrixLength + j] == 0
                        && neighbour_live_cell_11 == 3) {
                matrixALL[nextMatrix + i * matrixLength + j] = 1;
            } else {
                matrixALL[nextMatrix + i * matrixLength + j] = 0;
            }

            // bottom-left
            neighbour_live_cell_21 
                = countLiveNeighbour(matrixALL, timeCount, matrixLength, (i+1), j);
            if (matrixALL[currentMatrix + (i+1) * matrixLength + j] == 1
                    && (neighbour_live_cell_21 == 2 || neighbour_live_cell_21 == 3)) {
                matrixALL[nextMatrix + (i+1) * matrixLength + j] = 1;
            } 
            else if (matrixALL[currentMatrix + (i+1) * matrixLength + j] == 0
                        && neighbour_live_cell_21 == 3) {
                matrixALL[nextMatrix + (i+1) * matrixLength + j] = 1;
            }

            // top-right
            neighbour_live_cell_12 
                = countLiveNeighbour(matrixALL, timeCount, matrixLength, i, (j+1));
            if (matrixALL[currentMatrix + i * matrixLength + (j+1)] == 1
                    && (neighbour_live_cell_12 == 2 || neighbour_live_cell_12 == 3)) {
                matrixALL[nextMatrix + i * matrixLength + (j+1)] = 1;
            } 
            else if (matrixALL[currentMatrix + i * matrixLength + (j+1)] == 0
                        && neighbour_live_cell_12 == 3) {
                matrixALL[nextMatrix + i * matrixLength + (j+1)] = 1;
            }

            // bottom-right
            neighbour_live_cell_22 
                = countLiveNeighbour(matrixALL, timeCount, matrixLength, (i+1), (j+1));
            if (matrixALL[currentMatrix + (i+1) * matrixLength + (j+1)] == 1
                    && (neighbour_live_cell_22 == 2 || neighbour_live_cell_22 == 3)) {
                matrixALL[nextMatrix + (i+1) * matrixLength + (j+1)] = 1;
            } 
            else if (matrixALL[currentMatrix + (i+1) * matrixLength + (j+1)] == 0
                        && neighbour_live_cell_22 == 3) {
                matrixALL[nextMatrix + (i+1) * matrixLength + (j+1)] = 1;
            }

        }
    }

    // if the last row and col doesn't include in previous loop, doing here
    if((matrixLength-1)%2 == 0) {
        for(int i = 1; i < (matrixLength-2); i++) {
            neighbour_live_cell = countLiveNeighbour(matrixALL, timeCount, matrixLength, 
                                                        i, (matrixLength-2));
            if (matrixALL[currentMatrix + i * matrixLength + (matrixLength-2)] == 1
                    && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
                matrixALL[nextMatrix + i * matrixLength + (matrixLength-2)] = 1;
            } 
            else if (matrixALL[currentMatrix + i * matrixLength + (matrixLength-2)] == 0
                        && neighbour_live_cell == 3) {
                matrixALL[nextMatrix + i * matrixLength + (matrixLength-2)] = 1;
            }
        }

        for (int j = 1; j < (matrixLength-2); j++) {
            neighbour_live_cell = countLiveNeighbour(matrixALL, timeCount, matrixLength, 
                                                        (matrixLength-2), j);
            if (matrixALL[currentMatrix + (matrixLength-2) * matrixLength + j] == 1
                    && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
                matrixALL[nextMatrix + (matrixLength-2) * matrixLength + j] = 1;
            } 
            else if (matrixALL[currentMatrix + (matrixLength-2) * matrixLength + j] == 0
                        && neighbour_live_cell == 3) {
                matrixALL[nextMatrix + (matrixLength-2) * matrixLength + j] = 1;
            }
        }
        
        neighbour_live_cell = countLiveNeighbour(matrixALL, timeCount, matrixLength, 
                                                    (matrixLength-2), (matrixLength-2));
        if (matrixALL[currentMatrix + (matrixLength-2) * matrixLength + (matrixLength-2)] == 1
                && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
            matrixALL[nextMatrix + (matrixLength-2) * matrixLength + (matrixLength-2)] = 1;
        } 
        else if (matrixALL[currentMatrix + (matrixLength-2) * matrixLength + (matrixLength-2)] == 0
                    && neighbour_live_cell == 3) {
            matrixALL[nextMatrix + (matrixLength-2) * matrixLength + (matrixLength-2)] = 1;
        }
    }
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


int main (int argc, char** argv) 
{   
    // adding "walls" into the matrix length
    int totalLength = N+2;

    // initialise the matrix
    int* matrixAll = setup(totalLength, T, P);

    // getting and saving each timestep
    for (int i = 0; i < T; i++) {
        nextStep(matrixAll, i, totalLength);
    }

    // printing or saving data
    #ifdef ENABLE_HDF
        mat2hdf5(matrixAll, T, (totalLength * totalLength), "life.h5", "DS1");
    #else
        // printTimeSeries(matrixAll, T, totalLength);
    #endif

    free(matrixAll);
}