#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <pthread.h>
#include "etime.h"
#include <time.h>

//Opening a file

const char * FNAME = "kmeansLarge.txt";
const char * OUTNAME = "out.txt";

int main(){
FILE * ifp;
FILE * ofp;
int r;  // Saves the return value given by fscanf()
int c;
double** samples;
double** cluster_centers;
int j;
int k;
int num_of_clusters;
int dimensions;


ifp = fopen( FNAME, "r");
    if (ifp ==NULL ) {
    fprintf( stderr, "File '%s' not found.\n", FNAME);
    exit(1);
   }
ofp = fopen(OUTNAME, "w"); 
    if ( ofp == NULL ) {
    fprintf( stderr, "Can not write file '%s' .\n", OUTNAME);
    fclose(ifp);
    exit(2);
   }
  //get the number of samples and number of features
   fscanf(ifp, "%d", &r);
   num_of_clusters=r;
   samples = malloc(r*sizeof(double*));
   fscanf(ifp, "%d", &c);
   dimensions=c;
  
  printf("Number of samples %d, Number of dimensions %d\n", r,c);

 // allocate arrays
  for ( k=0; k<r; k++){
   samples[k]=malloc(c*sizeof(double*));
  }
 //read data into arrays
 for(k=0; k<r; k++)
{
     for( j=0; j<c; j++)
     {
     fscanf(ifp,"%lf",&samples[k][j]);
     }

} 
 
  // Choose K different random samples to use as center values
  

    int i;
    time_t timeValue;
    
    srand((unsigned) time(&timeValue));  // Seed kernel for random generator
    int x, dim;
    i=0;
    int randomint[num_of_clusters];
    // Loop to make sure there are no repeats
    while(i<num_of_clusters) {
        int p = rand()%r;
        for (x=0; x<i; x++) {   // Check whether random int has been used before
            if ( randomint[x]==p) {
                break;
            }
        }
        if (x==i) {
            randomint[i] = p;
            i++;
        }

    }


    // Now make an array with coordinates of randomly selected cluster centers
    for (i=0; i<num_of_clusters; i++) {
        for (dim=0; dim<dimensions; dim++) {
            cluster_centers[i][dim] = samples[randomint[i]][dim];
        }
          printf("%f, %f from sample %i\n",cluster_centers[i][0],cluster_centers[i][1],i);
    }


//calculate the distance of each sample from the cluster center

int threads_to_use;
int global_count;
pthread_mutex_t mutex;

void* Kmeans(void* rank); // Thread function

int main(int argc, char* argv[])
{
	long thread;
	pthread_t* thread_handles;

	global_count = 0;
	
	threads_to_use = strtol(argv[1], NULL, 10);

	thread_handles = malloc(threads_to_use*sizeof(pthread_t));

	pthread_mutex_init(&mutex,NULL);
	tic();
	for (thread = 0; thread < threads_to_use; thread++)
	{
		pthread_create(&thread_handles[thread], NULL, Kmeans, (void*)thread);
	}

	
	for (thread = 0; thread < threads_to_use; thread++)
	{
		pthread_join(thread_handles[thread], NULL);
	}
	toc();

	pthread_mutex_destroy(&mutex);
	free(thread_handles);
	printf("count = %d\n", global_count);
	printf("elapsed time = %g\n", etime()); 
	return 0;
}

void* Kmeans(void* rank)
{
	long my_rank = (long) rank;
	int chunk_size = num_of_clusters / threads_to_use;
	int my_start = my_rank * chunk_size + 1;
	int my_end = my_rank * chunk_size + chunk_size;
	if (my_rank == (threads_to_use-1)) my_end = num_of_clusters;
	int a, b;
        double* distance;

	for ( a = my_start ; a <= my_end ; a++ ) 
	{
		for ( b = a+1 ; b <= num_of_clusters ; b++ ) 
		{
			for (k=0; k<=num_of_clusters;k=k+2)
                       {
                          for(j=0;j<=dimensions;j=j+2)
                        {
                        distance[a]=sqrt((samples[k][j]-cluster_centers[k][j])*(samples[k][j]-cluster_centers[k][j])) ;
                        distance[b]=sqrt((samples[k+1][j+1]-cluster_centers[k+1][j+1])*(samples[k+1][j+1]-cluster_centers[k+1][j+1])) ;
		        pthread_mutex_lock(&mutex);
		        global_count++;
		        pthread_mutex_unlock(&mutex);
                        }
                      }
		}
	}

	return NULL;
}


}

