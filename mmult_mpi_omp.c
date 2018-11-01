#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
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
    nrows = atoi(argv[1]); //gets num rows from command line
    ncols = nrows;
    if (myid == 0) {
      // Master Code goes here
      aa = gen_matrix(nrows, ncols); //generates a matrics
      bb = gen_matrix(ncols, nrows);
      cc1 = malloc(sizeof(double) * nrows * nrows); 
      starttime = MPI_Wtime();
      /* Insert your master code here to store the product into cc1 */
	int i =0;
	int j =0;
	int k = 0;
	int l = 0; 
	for (i = 0; i < nrows; i++) {  
		for (j = 0; j < nrows; j++){    
			cc1[i][j] = 0;  
		}
		for (k = 0; k < nrows; k++){    
			for (l = 0; l < nrows; l++){      
		    		cc1[i][l] += aa[i][k] * bb[k][l];
				printf("cc1 = %d\n", cc1[i][l]); 
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
    } else {
      // Slave Code goes here
	  
	  //FROM POWERPOINT
	    /*
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
		
    }*/
  } else {
    fprintf(stderr, "Usage matrix_times_vector <size>\n");
  }
  MPI_Finalize();
  return 0;
}

