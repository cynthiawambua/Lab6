#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x)<(y)?(x):(y))

double* gen_matrix(int n, int m);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);

/** 
    Program to multiply a matrix times a matrix using both
    mpi to distribute the computation among nodes and omp
    to distribute the computation among threads.
*/

int main(int argc, char* argv[])
{
  int nrows, ncols;
  double *aa;	/* the A matrix */
  double *bb;	/* the B matrix */
  double *cc1;	/* A x B computed using the omp-mpi code you write */
  double *cc2;	/* A x B computed using the conventional algorithm */
  int myid, numprocs;
  double starttime, endtime;
  MPI_Status status;
  /* insert other global variables here */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  if (argc > 1) {
	int i =0;
	int j =0;
	int k = 0;
	int l = 0; 
    
	nrows = atoi(argv[1]); //gets num rows from command line
	ncols = nrows;
	//DID THIS WORK???
	/*
	double *aa[nrows];
	double *bb[nrows];
	double *cc1[nrows];	// A x B computed using the omp-mpi code you write 
    for(i=0; i<nrows; i++){
		cc1[i] = malloc(sizeof(double) * ncols);
	}
	*/
	
    if (myid == 0) {
      // Master Code goes here
      aa = gen_matrix(nrows, ncols); //generates a matrics
      bb = gen_matrix(ncols, nrows);
      cc1 = malloc(sizeof(double) * nrows * nrows); 
      starttime = MPI_Wtime();
      /* Insert your master code here to store the product into cc1 */
	
	for (i = 0; i < nrows; i++) {  
		for (j = 0; j < ncols; j++){    
			cc1[i*ncols + j] = 0;  
		}
		for (k = 0; k < nrows; k++){    
			for (l = 0; l < ncols; l++){      
		    		cc1[i*ncols + l] += aa[i*ncols + k] * bb[k*ncols + l];
				printf("cc1 = %lf\n", &cc1[i*ncols + l]); 
			}
		}
	}
	    
      endtime = MPI_Wtime();
      printf("%f\n",(endtime - starttime));
	  //creates test matrix that program will use to compare my calculated
	  //... matrix to what the answer should be
      cc2  = malloc(sizeof(double) * nrows * nrows);
      mmult(cc2, aa, nrows, ncols, bb, ncols, nrows);
	  //compares matrices 
      compare_matrices(cc2, cc1, nrows, nrows);
    } /*else {
      // Slave Code goes here
	  
	  //FROM POWERPOINT
	    
	  MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
	  if (myid <= nrows) {    
		while(1) {    
			//recieve a vector
			MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG, MPI_COMM_WORLD, &status);        
			if (status.MPI_TAG == 0){ break; }   
			//recieve the row
			row = status.MPI_TAG;        
			//initialize the answer 
			ans = 0.0; 
			//calculate the answer
			for (j = 0; j < ncols; j++) {            
				ans += buffer[j] * bb[j];        
			}
			//send answer back to the master 
			MPI_Send(&ans, 1, MPI_DOUBLE, master, row,MPI_COMM_WORLD);    
		}
		
	  
  } */}else {
    fprintf(stderr, "Usage matrix_times_vector <size>\n");
	}
  MPI_Finalize();
  return 0;
}


int mmult(double *c,double *a, int aRows, int aCols, 
	  double *b, int bRows, int bCols) 
{
  int i, j, k;
  for (i = 0; i < aRows; i++) {
    for (j = 0; j < bCols; j++) {
      c[i*bCols + j] = 0;
    }
    for (k = 0; k < aCols; k++) {
      for (j = 0; j < bCols; j++) {
	c[i*bCols + j] += a[i*aCols + k] * b[k*bCols + j];
      }
    }
  }
  return 0;
}

double* gen_matrix(int n, int m) {
  int i, j;
  double *a = malloc(sizeof(double) * n * m);
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      a[i*m + j] = (double)rand()/RAND_MAX;
    }
  }
  return a;
}

void compare_matrices(double* a, double* b, int nRows, int nCols) {
	int n = nRows * nCols;
	int i, j, k;
	for (k = 0; k < n; ++k) {
		if (fabs(a[k]-b[k])/fabs(a[k]) > 1e-12) {
			i = k/nCols;
			j = k%nCols;
			printf("a[%d][%d] == %.12g\nb[%d][%d] == %.12g\ndelta == %.12g\nrelerr == %.12g\n",
				i, j, a[k], i, j, b[k], fabs(a[k]-b[k]), fabs(a[k]-b[k])/fabs(a[k]));
			return;
		}
	}
	printf("Matrices are the same\n");
}

