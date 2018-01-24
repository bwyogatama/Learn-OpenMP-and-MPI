
#include <stdio.h>
#include <mpi.h>

// Deklarasi konstanta
#define SIZE 30 //  Ukuran matriks berdasarkan jumlah variabel persamaan linear

// Deklarasi variabel
    // Perkalian Persamaan Linear : Ax=b
    double A[SIZE][SIZE]; // Matriks koefisien
    double b[SIZE];// Vektor hasil
    double x[SIZE];// Vektor solusi yang dicari
    double c[SIZE];// Array rasio untuk proses eliminasi
    int map[SIZE];// Array pembagi tugas

int main(int argc, char **argv)
{
    
    // Deklarasi variabel
    int i,j,k;// Variabel loop
    int rank, proc;// Variabel program paralel
    double temp; // variabel penampung sementara
    double begin1, end1;// Variabel waktu
    MPI_Init(&argc, &argv);// Inisialisasi MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);// Label tiap proses
	MPI_Comm_size(MPI_COMM_WORLD, &proc);// Jumlah proses
	printf("%d\n",proc);  
	// Buat sembarang matriks di proses 0
    if (rank==0)
    {
        for (i=0; i<SIZE; i++)
        {
            for (j=0; j<SIZE; j++){
                A[i][j]=rand()%9;
            }
            b[i]=rand()%9;
        }
        
        // Cetak matriks koefisien (diketahui)
        printf("\nMatriks koefisien a00 sampai a%d%d :\n",SIZE-1,SIZE-1);
        for(i=0;i<SIZE;i++){
			for(j=0;j<SIZE;j++)
				printf("%.2f ",A[i][j]);
printf("\n");
		}
            
        // Cetak vektor hasil (diketahui)
        printf("\nVektor hasil b0 sampai b%d :\n",SIZE-1);
        for(i=0;i<SIZE;i++)
            printf("%.2f\n",b[i]);
    }

    begin1 = MPI_Wtime();//Penanda pengerjaan program paralel dimulai

    MPI_Bcast (&A[0][0],SIZE*SIZE,MPI_DOUBLE,0,MPI_COMM_WORLD);// Broadcast data matriks A[0][0] ke seluruh proses
    MPI_Bcast (b,SIZE,MPI_DOUBLE,0,MPI_COMM_WORLD);// Broadcast data vektor hasil B ke seluruh proses
	
	// membagi tugas sesuai jumlah proses
    for(i=0; i<SIZE; i++)
    {
        map[i]= i%proc; 
    } 
	
	// Eliminasi Gauss
    for(i=0;i<SIZE;i++)
    {
        MPI_Bcast (&A[i][i],SIZE-i,MPI_DOUBLE,map[i],MPI_COMM_WORLD);//Broadcast data matriks yang menjadi acuan baris
        MPI_Bcast (&b[i],1,MPI_DOUBLE,map[i],MPI_COMM_WORLD);// Broadcast data vektor hasil yang menjadi acuan baris     
        // Perhitungan rasio berdasarkan data posisi matriks yang menjadi acuan baris
        for(j= i+1; j<SIZE; j++) 
			// Lakukan pada proses sesuai pembagian tugas
			if(map[j] == rank)
				c[j]=A[j][i]/A[i][i];
		// Operasi matematika untuk proses eliminasi
        for(j= i+1; j<SIZE; j++)    
			// Lakukan pada proses sesuai pembagian tugas
			if(map[j] == rank)
				{
				for(k=0;k<SIZE;k++)
					A[j][k] -= c[j]*A[i][k];
				b[j] -= c[j]*b[i] ; 
				}
    }
    
	// Substitusi balik pada proses 0
    if (rank==0)
    { 
        x[SIZE-1]=b[SIZE-1]/A[SIZE-1][SIZE-1];
        for(i=SIZE-2;i>=0;i--)
        {
            temp = b[i];
			for(j=i+1;j<SIZE;j++){
                temp -= (A[i][j] * x[j]);
}
			x[i] = temp / A[i][i];
        }
    }
    
    end1 = MPI_Wtime();//Penanda pengerjaan program paralel selesai
	
	// Cetak semua matriks dan waktu eksekusi pada proses 0
    if (rank==0)
    {            
	    // Cetak vektor solusi (dicari)
        printf("\nVektor solusi x0 sampai x%d (dicari):\n",SIZE-1);
        for(i=0;i<SIZE;i++)
            printf("%.2f\n",x[i]);
	
	    // Cetak waktu eksekusi
        printf("\nWaktu eksekusi: %f\n", end1-begin1);
	printf("%d %d\n", rank, proc);
    }
    
    // Akhiri MPI
    MPI_Finalize();
    return(0);

}
