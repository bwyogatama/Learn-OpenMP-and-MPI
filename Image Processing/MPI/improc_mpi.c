/* Extra Credit Homework 1 Solution (#40)                                     */
/* Yudi Gondokaryono                                                          */
/* Section 1&2                                                                */
/* November 11, 2002                                                          */
/******************************************************************************/
/* Nama		: Bobbi Winema Yogatama											  */
/* NIM		: 13214071														  */
/******************************************************************************/
/* Purpose : Imgage Filtering Program                                         */
/******************************************************************************/
/* Variable Definitions (Main Function):                                      */
/* Variable Name          Type     Description                                */
/* pict[256][256]         int      image array                                */
/* new_pict[256][256]     int      new image array                            */
/* r                      int      # of row in the image                      */
/* c                      int      # of column in the image                   */
/* flt[3][3]              double   filter array                               */
/******************************************************************************/
/* Source Code:                                                               */

/* Include all library we need                                                */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

/* Declare all constant                                                       */
#define IM_SIZE 256 // maximum image size
#define FILTER_SIZE 3 // filter size
//declare variable needed in mpi
#define MASTER_TO_SLAVE_TAG 1 //tag for messages sent from master to slaves
#define SLAVE_TO_MASTER_TAG 4 //tag for messages sent from slaves to master
int low_bound; /*low bound of the number of rows allocated to a slave*/
int upper_bound; /*upper bound of the number of rows allocated to a slave*/
int portion; /*portion of the number of rows allocated to a slave*/
MPI_Status status; // store status of a MPI_Recv
MPI_Request request; //capture request of a MPI_Isend

/* Declare all function prototype                                             */
void read_pict(int pict[][IM_SIZE], int *r, int *c);
void image_filter(int pict[][IM_SIZE], int r, int c, double filter[][FILTER_SIZE],
				  int new_pict[][IM_SIZE], int npes, int myrank);
void write_pict(int pict[][IM_SIZE], int r, int c);

/* Begin the Main Function                                                    */
int main(int argc, char *argv[])
{
	double t;
	int npes, myrank;
	int width, height; // actual image size
	int pict[IM_SIZE][IM_SIZE];
	int new_pict[IM_SIZE][IM_SIZE];
	double flt[FILTER_SIZE][FILTER_SIZE]={{-1.25, 0, -1.25},
										  {0, 10, 0},
									   	  {-1.25, 0, -1.25}};
	
	//Inisialisasi MPI
	MPI_Init(&argc,&argv);
	//mengecek jalannya mpi
    MPI_Comm_size(MPI_COMM_WORLD,&npes);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

    /* Start timer */
    t = MPI_Wtime();
    
    /*Inisialisasi master*/
    if (myrank == 0) 
    {
		read_pict(pict, &width, &height);
	}

	MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	image_filter(pict, width, height, flt, new_pict, npes, myrank);

	if (myrank == 0) 
    {
		write_pict(new_pict, width, height);
	}

	/* Stop timer, includes communication time */
	t = MPI_Wtime() - t;
	printf("Waktu proses: %f detik\n", t);

    MPI_Finalize();

	return(0);
/* End Main Function                                                          */
}

/* Begin read_pict function                                                   */
/******************************************************************************/
/* Purpose : This function reads the image from original.pgm                  */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* r                      int *    # of rows pointer                          */
/* c                      int *    # of column pointer                        */
/* max                    int      maximum pixel value                        */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* line[200]              char     string                                     */
/* in                     FILE *   input file pointer                         */
/******************************************************************************/
/* Source Code:                                                               */
void read_pict(int pict[][IM_SIZE], int *r, int *c)
{
	int i, j, max;
	FILE *in;
	char line[200];
	
	in=fopen("sector.pgm", "r");	// load gambar Sheldon kya
	if(in == NULL)
	{
		printf("Error reading sector.pgm\n");
		exit(1);
	}
	
	fgets(line, 199, in); // get PGM Type
	
	line[strlen(line)-1]='\0';  // get rid of '\n'
	
	// if(strcmp(line, "P2") != 0)
	// {
	// 	printf("Cannot process %s PGM format, only P2 type\n", line);
	// 	// exit(2);
	// }
	
	fgets(line, 199, in); // get comment
		
	fscanf(in, "%d %d", c, r); // get size width x height
	
	fscanf(in, "%d", &max); // get maximum pixel value

	for(i = 0; i < *r; i++)
		for(j = 0; j < *c; j++)
			fscanf(in, "%d", &pict[i][j]);
	fclose(in);

	return;
/* End read_pict function                                                     */
}	

/* Begin image_filter function                                                */
/******************************************************************************/
/* Purpose : This function filter the image and create a new image            */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* new_pict[][]           int      array address                              */
/* r                      int      # of rows                                  */
/* c                      int      # of column                                */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* m                      int      loop counter                               */
/* n                      int      loop counter                               */
/* coeff                  double   coefficient                                */
/* sum                    double   sum                                        */
/******************************************************************************/
/* Source Code:                                                               */
void image_filter(int pict[][IM_SIZE], int r, int c, double filter[][FILTER_SIZE],
				  int new_pict[][IM_SIZE], int npes, int myrank)
{
	double coeff = 0;
	double sum;
	int i, j, m, n;
	
	if(myrank==0)
	{
	/*  compute coefficient                                                       */	
		for(i = 0; i < FILTER_SIZE; i++)
			for(j = 0; j < FILTER_SIZE; j++)
				coeff += filter[i][j];

		for (i = 1; i < npes; i++) 
		{//for each slave other than the master
            portion = (r / (npes - 1)); // calculate portion without master
            low_bound = (i - 1) * portion;

            if (((i + 1) == npes) && ((IM_SIZE % (npes - 1)) != 0)) 
            {//if rows of [A] cannot be equally divided among slaves
                upper_bound = IM_SIZE; //last slave gets all the remaining rows
            } 
            else 
            {
                upper_bound = low_bound + portion; //rows of [A] are equally divisable among slaves
            }

            /* Fix */
            if(low_bound!=0) low_bound = low_bound - 1;
            if(upper_bound!=r) upper_bound = upper_bound + 1;

            //send the low bound first without blocking, to the intended slave
            MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
            //next send the upper bound without blocking, to the intended slave
            MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
            //finally send the allocated row portion of [A] without blocking, to the intended slave
            MPI_Isend(&pict[low_bound][0], (upper_bound - low_bound) * c, MPI_INT, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
		}		
	}

	MPI_Bcast(&coeff, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); 

    /* work done by slaves*/
    if (myrank > 0) 
    {
        //receive low bound from the master
        MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
        //next receive upper bound from the master
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
        //finally receive row portion of [A] to be processed from the master
        MPI_Recv(&pict[low_bound][0], (upper_bound - low_bound) * c, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);


	/*  filter the image                                                          */
		for(i = low_bound+1; i < upper_bound-1; i++)
		{
			for(j = 1; j < c-1; j++)
			{
				sum = 0;
				for(m = 0; m < FILTER_SIZE; m++)
					for(n = 0; n < FILTER_SIZE; n++)
						sum += pict[i+(m-1)][j+(n-1)]*filter[m][n];
				new_pict[i][j] = (int)sum;
			} 

		}
		
		if(coeff != 0)
		{
			for(i = low_bound+1; i < upper_bound-1; i++)
				for(j = 1; j < c-1; j++)
				{
					new_pict[i][j] = (int)(new_pict[i][j]/coeff);
					/*  check for pixel > 255 and pixel < 0                                       */
					if(new_pict[i][j] < 0)
						new_pict[i][j] = 0;
					else if(new_pict[i][j] > 255)
						new_pict[i][j] = 255;
				}
		}
		
        /* Fix */
        if(low_bound!=0) low_bound = low_bound + 1;
        if(upper_bound!=r) upper_bound = upper_bound - 1;

        //send back the low bound first without blocking, to the master
        MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
        //send the upper bound next without blocking, to the master
        MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
        //finally send the processed portion of data without blocking, to the master
        MPI_Isend(&new_pict[low_bound][0], (upper_bound - low_bound) * c, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
    }

    /* master gathers processed work*/
        if (myrank == 0) 
        {
            for (i = 1; i < npes; i++) {// until all slaves have handed back the processed data
                //receive low bound from a slave
                MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
                //receive upper bound from a slave
                MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
                //receive processed data from a slave
                MPI_Recv(&new_pict[low_bound][0], (upper_bound - low_bound) * c, MPI_INT, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
            }

			/*  copy edges                                                                */
			for(i = 0; i < r; i++)
			{
				new_pict[i][0] = pict[i][0];
				new_pict[i][c-1] = pict[i][c-1];
			}

			for(j = 0; j < c; j++)
			{
				new_pict[0][j] = pict[0][j];
				new_pict[r-1][j] = pict[r-1][j];
			}
		}

	return;
/* End image_filter function                                                  */
}

/* Begin write_pict function                                                  */
/******************************************************************************/
/* Purpose : This function write the filtered image to new.pgm                */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* new_pict[][]           int      array address                              */
/* r                      int      # of rows                                  */
/* c                      int      # of column                                */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* out                    FILE *   output FILE pointer                        */
/******************************************************************************/
/* Source Code:                                                               */
void write_pict(int pict[][IM_SIZE], int r, int c)
{
	int i, j;
	FILE *out;
	out=fopen("sektorhasil.pgm", "w");

	fprintf(out, "P2\n");
	fprintf(out, "# sektorhasil.pgm\n");
	fprintf(out, "%d %d\n", r, c);
	fprintf(out, "255\n");
	
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
			fprintf(out, "%5d", pict[i][j]);
		fprintf(out, "\n");
	}
	
	fclose(out);
	return;
/* End write_pict function                                                    */
}

