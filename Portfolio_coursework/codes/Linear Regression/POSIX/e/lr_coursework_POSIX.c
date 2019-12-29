#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include<time.h>

/*******************************************************************

* To compile:
 *   cc -o lr_coursework_POSIX lr_coursework_POSIX.c -lm -pthread
 * 
 * To run:
 *   ./lr_coursework_POSIX


*************************************************************/


int time_difference(struct timespec *start, struct timespec *finish, 
                    long long int *difference) {
                       long long int ds =  finish->tv_sec - start->tv_sec; 
                       long long int dn =  finish->tv_nsec - start->tv_nsec; 

                       if(dn < 0 ) {
                          ds--;
                          dn += 1000000000; 
                       } 
                       *difference = ds * 1000000000 + dn;
                       return !(*difference > 0);
}


//struct definition for point (x,y)
typedef struct {
  double x;
  double y;
} point_t;

//struct definition for passing as argument to the tread function 
//variable startingIndex serves for initialization expression in FOR loop inside thread function 
//variable errorSumIndex serves for ideintifying error_sum for a particular thread computation
struct blocksize{
  int startingIndex;
  int errorSumIndex;
};

//since 8 threads are used there needs to be 8 different instances of  blocksize struct data type
struct blocksize block[8];

int n_data = 1000;

//since slope and intercept are same for each instance of thread while computing residual error these
//are declared as global variable
double slope, intercept;  

//each 8 thread computes residual_error for 125 data points therefore array of 8 er_sum is required
//to hold sum of error computed by 8 diferent threads
double er_sum[8];

point_t data[];


double residual_error(double x, double y) {
  double e = (slope * x) + intercept - y;
  return e * e;
}


//sequential looping portion from lr05.c program has been taken out and placed inside thread function 
//called thread_function 
void * thread_function(void *ptr){
	struct blocksize *bsize=(struct  blocksize *)ptr;
	
	//initialization of er_sum to 0 to ensure that it doesnt hold garbage 
	  er_sum[bsize->errorSumIndex]=0.00;
	  
	  //since 8 thread will called this function each thread will process 1000/8=125 set of points
	for(int i=bsize->startingIndex; i<((bsize->startingIndex)+125); i++) {
		
    	er_sum[bsize->errorSumIndex] += residual_error(data[i].x, data[i].y);  
		
	}
	pthread_exit(NULL);
	
}

double rms_error(double m, double c) {
  slope=m;
  intercept=c;
  pthread_t t[8];

  double mean;
  double total_error_sum = 0.00;

	int x=0;
  for (int l=0;l<8;l++){
	  
	  block[l].errorSumIndex=l;
	  
	  //updating starting index for each thread before being called
	  block[l].startingIndex=x;
	  
	  pthread_create(&t[l],NULL,thread_function,&block[l]);
	  
	  x+=125;
  }
  
  for (int l=0;l<8;l++){
	  pthread_join(t[l],NULL);
	  
	  //ensuring that thread are joined and summing individual sum computed by 8 different threads
	  //into total_error_sum
	  total_error_sum+=er_sum[l];
  
  }
  
  mean = total_error_sum / n_data;
  
  return sqrt(mean);
}

int main() {
  int i;
  double bm = 1.3;
  double bc = 10;
  double be;
  double dm[8];
  double dc[8];
  double e[8];
  double step = 0.01;
  double best_error = 999999999;
  int best_error_i;
  int minimum_found = 0;
struct timespec start, finish;   
  long long int time_elapsed;
  
  double om[] = {0,1,1, 1, 0,-1,-1,-1};
  double oc[] = {1,1,0,-1,-1,-1, 0, 1};

clock_gettime(CLOCK_MONOTONIC, &start);

  be = rms_error(bm, bc);
printf("value of BE is %lf\n",be);
  while(!minimum_found) {
    for(i=0;i<8;i++) {
      dm[i] = bm + (om[i] * step);
      dc[i] = bc + (oc[i] * step);    
    }
      
    for(i=0;i<8;i++) {
      e[i] = rms_error(dm[i], dc[i]);
      if(e[i] < best_error) {
        best_error = e[i];
        best_error_i = i;
      }
    }

   // printf("best m,c is %lf,%lf with error %lf in direction %d\n", 
     // dm[best_error_i], dc[best_error_i], best_error, best_error_i);
    if(best_error < be) {
      be = best_error;
      bm = dm[best_error_i];
      bc = dc[best_error_i];
    } else {
      minimum_found = 1;
    }
  }
  printf("minimum m,c is %lf,%lf with error %lf\n", bm, bc, be);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed, 
         (time_elapsed/1.0e9));


  return 0;
}

point_t data[] = {
  {72.79,133.63},{65.97,108.84},{73.57,112.37},{68.44,116.26},
  {69.77,134.42},{82.26,150.74},{90.52,151.45},{65.54,112.73},
  {78.00,141.51},{65.01,115.87},{78.24,137.01},{89.72,168.27},
  { 6.41,29.29},{41.94,78.67},{87.57,150.57},{49.92,103.74},
  {88.08,149.06},{23.10,48.11},{80.16,142.92},{68.16,131.55},
  {79.65,149.88},{34.86,75.67},{33.05,81.21},{39.53,66.68},
  { 0.44,22.55},{ 4.58,29.60},{43.33,89.81},{14.75,55.18},
  {69.98,132.18},{ 3.77,23.06},{89.23,170.21},{92.04,155.78},
  {97.71,165.90},{94.65,179.68},{53.06,105.93},{17.17,43.75},
  { 5.98,27.26},{31.88,74.13},{16.56,40.43},{ 2.42,14.63},
  {67.98,115.60},{14.89,35.72},{18.70,43.34},{12.62,53.98},
  {98.80,171.84},{68.10,126.07},{50.61,90.46},{83.49,140.37},
  {41.04,64.55},{ 9.50,38.55},{22.79,36.30},{28.66,52.62},
  {81.64,158.52},{97.34,167.65},{22.71,59.57},{90.31,160.42},
  {44.79,82.58},{ 3.04,29.45},{13.90,27.06},{23.21,49.92},
  { 7.44,30.07},{69.49,127.49},{37.82,74.45},{54.54,90.22},
  {10.89,40.22},{29.74,59.02},{23.06,40.16},{32.07,75.67},
  {58.53,115.82},{14.04,44.07},{95.08,164.71},{74.31,137.20},
  {14.15,37.72},{65.91,125.14},{66.17,122.21},{19.66,61.27},
  {80.70,141.25},{36.43,87.08},{71.67,134.96},{86.98,137.30},
  {55.33,101.13},{71.58,135.54},{72.84,129.92},{69.30,123.43},
  {42.64,81.40},{37.95,74.53},{71.42,139.15},{70.66,138.09},
  { 5.43,42.19},{62.18,113.61},{27.88,90.68},{39.87,84.92},
  {52.25,110.86},{91.41,167.32},{83.48,137.24},{49.62,95.93},
  {30.20,63.51},{73.10,127.17},{43.01,86.17},{48.41,99.41},
  {80.77,141.15},{39.91,83.57},{28.59,60.58},{87.39,170.70},
  { 5.62,25.72},{28.61,57.15},{ 6.80,21.20},{19.28,46.87},
  {51.43,95.72},{32.88,58.30},{29.42,62.16},{17.39,37.84},
  {32.37,71.52},{18.82,48.16},{32.70,83.04},{98.59,167.04},
  {17.29,59.01},{54.47,107.45},{33.16,86.70},{63.63,109.30},
  {94.70,176.10},{72.66,130.00},{38.09,68.30},{59.85,121.12},
  {15.20,47.75},{67.15,125.68},{34.49,77.89},{12.79,49.01},
  {63.20,114.17},{88.31,145.08},{94.08,154.06},{17.13,35.64},
  {78.14,150.37},{34.69,66.26},{63.10,123.38},{70.62,131.17},
  {30.74,55.36},{99.95,164.83},{47.52,91.44},{75.69,123.92},
  { 9.91,43.25},{55.02,97.42},{97.46,172.11},{73.10,121.40},
  {47.85,79.63},{98.65,164.01},{68.22,105.17},{55.89,108.94},
  {31.82,67.06},{88.28,155.71},{12.48,43.62},{26.80,67.85},
  {61.11,112.87},{46.55,102.66},{ 8.26,34.68},{40.95,83.26},
  {28.07,87.53},{58.18,103.27},{18.08,29.97},{70.90,120.05},
  { 0.38,17.62},{64.02,115.67},{12.90,37.53},{87.97,152.75},
  {56.93,87.35},{94.71,159.25},{21.24,45.39},{23.90,54.26},
  {25.62,68.19},{42.05,103.04},{41.15,82.08},{95.05,162.18},
  {50.95,100.52},{16.76,50.93},{38.19,84.01},{17.52,36.71},
  {82.86,153.15},{ 7.75,55.47},{61.84,124.29},{95.60,166.87},
  {56.43,107.41},{64.76,109.81},{80.66,141.78},{28.07,65.17},
  {85.84,147.10},{45.99,104.96},{99.66,165.66},{46.66,85.33},
  {73.13,122.14},{72.77,144.42},{74.04,127.51},{63.18,125.56},
  { 3.23,29.36},{13.29,37.13},{30.92,67.55},{54.04,104.23},
  {92.88,141.23},{18.08,45.06},{28.09,62.22},{40.27,89.91},
  {28.25,68.07},{ 5.73,33.58},{21.80,59.55},{97.21,165.76},
  {61.32,109.72},{48.07,98.25},{ 7.79,30.88},{42.43,86.44},
  {77.40,145.35},{94.38,148.66},{77.32,132.36},{ 2.46,43.25},
  {58.74,108.22},{66.30,108.14},{43.11,91.74},{92.59,172.88},
  {89.70,157.06},{14.36,27.22},{ 3.99,27.66},{24.79,44.19},
  {94.95,149.56},{ 7.52,23.71},{62.44,111.89},{81.65,132.88},
  {92.88,169.11},{97.34,169.49},{41.69,85.09},{21.29,64.39},
  {39.00,94.60},{ 4.28,26.88},{28.03,68.69},{26.23,71.77},
  {66.20,140.20},{60.24,113.95},{79.97,144.34},{13.36,27.16},
  {10.38,55.26},{92.55,157.68},{92.93,168.16},{25.20,49.77},
  {84.16,161.49},{45.97,107.56},{48.31,103.43},{76.86,132.30},
  {23.30,65.35},{22.40,60.41},{16.11,24.58},{52.97,83.19},
  {58.99,103.54},{84.49,153.12},{82.50,127.49},{54.50,104.39},
  {38.45,70.45},{ 3.79,27.91},{40.17,59.82},{59.19,112.19},
  { 6.64,30.54},{22.42,61.99},{72.66,107.30},{39.08,85.51},
  {71.39,141.06},{32.75,78.42},{25.19,64.25},{49.81,93.95},
  {62.31,112.76},{60.34,128.46},{56.91,108.35},{37.47,90.16},
  {80.49,131.46},{60.66,127.37},{42.28,93.38},{65.15,120.19},
  {10.53,43.64},{94.26,164.08},{32.85,72.10},{ 4.08,29.55},
  {28.49,66.51},{44.07,92.16},{ 0.47, 7.79},{16.56,69.72},
  {43.99,83.49},{33.40,60.17},{58.99,128.97},{ 9.65,29.84},
  {82.80,139.88},{74.19,135.48},{53.05,97.94},{ 4.24,30.49},
  {93.34,185.59},{69.84,121.95},{38.12,83.91},{ 7.09,33.19},
  {27.79,73.03},{57.41,121.47},{34.35,62.09},{85.94,156.97},
  {26.90,75.62},{91.72,161.70},{72.77,138.07},{77.30,143.62},
  {37.70,94.95},{39.03,81.39},{96.35,173.49},{63.13,112.12},
  {84.11,146.78},{ 1.02,33.73},{36.23,64.08},{61.03,99.25},
  {19.00,62.14},{10.34,35.16},{20.62,51.06},{84.02,155.13},
  {53.77,101.40},{94.49,170.55},{15.31,45.26},{89.66,161.43},
  {47.58,90.64},{61.27,139.24},{49.25,99.56},{18.69,56.20},
  {74.17,143.02},{86.63,164.49},{24.25,44.27},{27.10,75.06},
  {29.52,63.02},{26.96,52.84},{68.62,131.83},{88.67,154.58},
  {39.60,87.88},{92.79,189.61},{59.38,104.30},{31.33,82.48},
  {40.54,87.71},{96.84,163.83},{20.80,45.73},{63.28,109.41},
  {26.42,51.11},{24.90,50.69},{ 4.89,34.05},{66.46,126.10},
  {70.53,125.25},{57.88,112.00},{52.10,108.99},{68.52,142.58},
  {49.91,99.99},{40.53,92.74},{26.85,68.07},{47.71,102.72},
  {14.15,59.67},{45.83,81.29},{46.47,87.56},{42.95,71.00},
  {82.16,133.00},{ 3.38,23.01},{54.45,129.03},{35.65,84.59},
  {57.88,110.25},{61.98,118.96},{16.30,53.87},{58.46,111.68},
  {16.95,75.60},{49.63,86.49},{65.71,107.71},{88.00,152.50},
  {51.21,106.62},{12.91,62.43},{47.83,97.56},{94.20,177.05},
  {86.97,164.58},{ 4.90,17.45},{34.91,81.03},{83.56,144.85},
  {13.39,39.51},{16.98,50.93},{65.17,117.63},{64.62,93.13},
  {96.57,172.56},{94.38,166.17},{63.49,110.40},{47.24,97.93},
  {54.48,106.64},{82.50,144.59},{96.06,172.02},{42.78,104.61},
  {26.94,55.02},{62.46,119.10},{25.75,52.04},{22.19,44.93},
  { 0.91,20.64},{97.22,183.06},{62.18,104.02},{23.45,61.36},
  {91.49,156.55},{48.88,99.11},{42.00,81.11},{92.47,161.80},
  {95.56,178.64},{88.15,155.48},{96.40,161.35},{55.80,110.64},
  {34.95,75.89},{70.68,132.51},{19.56,48.90},{30.93,63.27},
  { 1.13,36.61},{17.18,47.20},{24.71,57.38},{87.36,144.44},
  {31.86,70.40},{86.44,152.36},{82.95,131.84},{86.09,141.70},
  {73.76,98.25},{ 8.72,40.30},{70.09,120.86},{51.16,100.44},
  { 1.26,26.29},{10.41,37.41},{55.22,119.37},{53.06,108.34},
  {14.20,44.40},{15.63,50.51},{69.38,126.21},{69.12,126.60},
  {37.68,94.88},{24.26,64.22},{27.41,65.48},{66.61,102.05},
  {13.97,56.86},{56.60,119.93},{92.21,166.29},{58.18,106.64},
  {65.30,128.46},{55.01,95.98},{43.70,87.54},{91.76,160.75},
  {30.79,74.59},{58.64,107.12},{27.33,59.84},{ 9.17,40.73},
  {91.39,157.17},{ 1.33,17.73},{98.50,178.07},{33.80,61.56},
  {80.12,138.99},{28.19,71.11},{30.96,62.95},{10.51,53.58},
  {29.41,56.54},{42.18,84.03},{94.40,163.46},{92.60,152.29},
  {83.49,140.39},{73.62,132.46},{42.50,93.81},{29.20,57.65},
  {15.37,44.99},{15.60,31.46},{43.94,74.58},{17.18,29.07},
  {87.80,168.78},{18.37,61.21},{57.04,118.97},{86.04,125.57},
  {62.62,111.77},{95.05,154.71},{73.82,131.07},{89.02,146.01},
  {84.55,147.22},{39.55,72.10},{76.28,128.65},{67.23,133.12},
  {41.68,79.83},{ 2.47,13.05},{13.34,33.76},{36.61,70.84},
  {77.91,133.83},{88.00,156.20},{68.03,110.03},{95.85,156.31},
  {84.44,127.93},{19.11,49.52},{77.77,135.22},{58.21,118.17},
  {94.48,156.24},{96.54,149.48},{97.17,173.39},{56.74,100.53},
  {61.42,116.37},{85.17,150.99},{10.57,50.02},{47.43,112.86},
  {59.82,104.72},{80.76,156.04},{10.04,45.54},{20.58,53.71},
  {52.52,112.40},{40.97,78.93},{55.33,106.90},{ 0.75,19.10},
  {56.11,110.27},{78.35,143.83},{51.13,98.66},{ 0.55,22.38},
  {33.68,57.40},{69.95,113.22},{49.81,97.35},{40.96,86.56},
  {82.85,139.64},{62.30,129.70},{33.68,73.54},{ 7.61,31.09},
  { 6.37,16.63},{37.19,81.41},{18.03,28.55},{ 5.34,39.28},
  {75.22,124.29},{56.26,123.11},{ 1.46,21.49},{80.08,128.54},
  {56.31,97.75},{14.73,54.18},{87.13,156.92},{83.07,169.01},
  {12.10,43.92},{56.99,99.14},{65.57,122.07},{67.18,114.44},
  {23.93,55.51},{72.19,130.41},{55.61,109.94},{89.18,163.93},
  {20.92,49.49},{98.45,169.21},{ 4.70,17.52},{53.47,105.60},
  {87.04,162.61},{43.74,88.02},{78.19,132.85},{96.22,160.88},
  {50.95,104.80},{30.72,63.85},{44.49,92.77},{63.43,129.91},
  {35.59,80.47},{83.05,136.55},{65.14,115.55},{85.56,141.45},
  {37.10,70.33},{41.11,64.70},{51.79,101.64},{76.63,137.64},
  {93.31,148.28},{79.42,134.96},{25.17,40.20},{54.35,92.98},
  {74.07,140.21},{56.58,104.13},{ 6.63,32.18},{30.70,61.47},
  {11.11,42.11},{84.92,146.74},{63.87,117.01},{60.21,125.12},
  {99.41,159.77},{69.53,115.93},{58.52,107.08},{92.45,141.85},
  { 6.84,38.88},{60.57,101.11},{70.53,129.67},{92.40,168.67},
  { 3.34,37.38},{53.44,104.47},{35.83,88.44},{38.21,80.65},
  {58.43,90.61},{27.90,75.98},{38.48,72.74},{37.09,81.52},
  {65.40,121.63},{54.03,102.55},{96.61,160.09},{91.84,169.88},
  { 6.71,28.32},{35.90,73.08},{93.36,167.00},{79.48,138.17},
  {19.85,38.47},{86.41,159.82},{73.77,126.20},{33.46,74.88},
  {56.54,109.60},{ 5.75,42.44},{52.27,115.29},{24.08,60.94},
  {79.60,142.19},{23.57,65.70},{13.91,40.15},{17.64,53.18},
  {54.96,111.56},{49.65,96.22},{74.51,133.63},{52.99,95.42},
  {46.33,84.66},{71.22,129.18},{49.71,91.83},{33.15,79.45},
  {45.28,94.02},{93.90,173.36},{84.00,146.31},{51.50,102.51},
  {56.60,88.16},{46.32,107.83},{ 3.42,16.54},{21.96,55.54},
  {58.23,96.02},{ 2.59,22.52},{28.37,51.98},{38.89,81.07},
  {21.83,60.13},{ 8.94,51.72},{86.64,152.78},{17.17,54.99},
  {64.32,131.49},{58.10,121.18},{46.20,80.84},{56.19,121.72},
  {75.21,138.59},{36.15,72.24},{79.94,155.91},{ 7.68,48.69},
  {89.80,159.95},{69.94,127.85},{85.47,137.10},{44.87,85.13},
  {10.30,22.08},{37.77,66.57},{80.13,140.30},{94.78,158.11},
  {76.39,121.93},{60.11,119.31},{63.01,119.33},{76.78,136.61},
  {85.48,146.64},{44.31,78.46},{51.50,99.24},{85.28,148.69},
  {78.32,154.99},{45.21,67.96},{39.25,74.48},{23.58,54.34},
  {15.80,24.98},{58.19,111.13},{69.35,138.97},{32.35,53.66},
  {30.30,57.47},{94.56,166.24},{59.32,106.33},{76.54,141.72},
  {26.30,69.04},{28.66,71.72},{83.81,156.22},{26.81,69.01},
  {75.24,146.81},{32.89,65.04},{20.79,42.22},{32.88,75.96},
  {31.61,67.95},{61.55,104.43},{18.89,48.57},{96.16,167.48},
  { 0.31,18.47},{73.11,122.34},{12.10,36.86},{22.96,38.88},
  {86.94,159.41},{31.14,73.21},{12.86,30.15},{29.94,71.92},
  {35.00,77.54},{43.87,102.89},{43.06,91.04},{74.87,118.14},
  { 1.10,24.19},{48.82,110.77},{33.18,73.13},{ 3.73,32.55},
  {70.42,123.02},{61.61,99.46},{39.99,68.47},{42.83,78.92},
  {77.91,145.69},{13.06,39.33},{ 7.50,23.63},{43.46,90.47},
  {42.93,84.57},{ 2.03,30.25},{60.49,105.80},{15.53,27.88},
  {54.74,124.56},{70.65,138.03},{25.10,47.64},{94.26,185.75},
  {38.17,97.40},{92.80,170.02},{21.20,39.85},{67.59,114.41},
  {64.11,120.63},{83.76,145.77},{19.35,55.64},{36.16,61.56},
  {90.37,167.90},{21.39,72.92},{58.29,110.80},{42.35,96.94},
  {52.76,100.77},{84.51,147.92},{90.59,156.33},{ 5.29,41.39},
  {10.43,32.77},{13.27,34.56},{69.39,124.12},{42.12,95.07},
  {67.27,114.34},{66.78,129.16},{56.73,114.36},{83.43,142.03},
  {13.73,57.56},{32.56,76.86},{63.88,135.52},{26.32,68.97},
  {31.65,80.33},{74.77,135.40},{91.76,152.01},{87.34,154.03},
  {56.42,112.76},{48.37,90.82},{60.00,106.66},{42.12,99.21},
  {39.87,76.32},{19.58,59.45},{97.72,155.11},{57.40,104.91},
  {31.85,73.38},{25.21,50.59},{52.19,102.28},{65.35,122.77},
  {90.51,158.17},{22.08,42.05},{ 9.71,34.61},{66.26,118.73},
  {90.45,149.53},{32.33,76.25},{16.48,25.16},{56.03,123.63},
  {94.49,183.19},{ 9.22,33.79},{76.84,158.84},{79.79,143.77},
  {35.92,97.34},{84.01,152.72},{ 7.46,33.16},{10.43,27.05},
  {30.50,62.71},{ 8.31,48.23},{ 7.84,23.12},{ 6.82,31.22},
  {65.04,111.89},{27.99,62.28},{20.64,41.28},{56.44,105.97},
  {50.93,113.77},{46.53,88.72},{47.51,91.63},{70.30,148.03},
  {25.67,65.21},{41.66,93.65},{69.89,122.12},{31.33,66.22},
  {99.70,177.76},{85.25,128.41},{25.35,47.29},{47.60,96.61},
  {81.06,136.96},{19.06,55.30},{72.78,142.32},{30.53,62.11},
  {87.93,145.74},{27.40,53.12},{53.25,107.75},{12.50,25.53},
  {64.44,100.68},{19.21,58.61},{49.55,98.08},{88.11,149.89},
  {11.28,33.63},{66.78,108.97},{ 9.81,29.71},{ 0.66,20.04},
  {94.03,169.97},{31.36,48.78},{97.18,168.28},{67.30,104.96},
  {56.85,107.49},{96.49,155.16},{27.67,56.79},{94.84,155.42},
  {33.62,76.66},{40.48,110.05},{31.93,63.52},{24.85,50.02},
  {85.58,150.32},{51.48,75.79},{87.54,152.34},{ 5.03,37.35},
  {26.00,42.88},{22.91,63.82},{14.95,54.45},{27.91,53.19},
  {63.74,117.02},{54.40,105.46},{61.65,112.88},{74.65,129.16},
  {42.18,75.21},{66.59,121.36},{98.63,174.74},{89.75,149.08},
  {40.37,79.59},{59.34,115.94},{39.23,46.85},{69.06,126.85},
  {34.26,77.31},{43.99,85.74},{34.17,63.61},{98.96,169.19},
  {69.94,139.39},{23.34,48.55},{60.89,96.00},{52.21,102.17},
  {31.01,65.90},{ 4.21,29.86},{40.45,78.66},{10.80,37.54},
  {20.84,52.39},{98.17,150.88},{82.98,124.83},{15.92,57.72},
  {98.32,162.69},{39.66,80.52},{79.19,125.55},{88.53,153.59},
  {72.29,128.45},{30.52,70.56},{56.09,96.32},{60.03,111.08},
  {86.88,178.70},{19.60,42.33},{28.61,56.02},{62.44,124.67},
  {48.83,88.66},{92.73,187.76},{63.44,132.76},{ 0.23,24.67},
  {87.28,169.96},{37.71,79.55},{59.58,105.30},{61.78,110.97},
  {53.96,100.66},{ 6.04,26.06},{38.45,57.76},{59.08,103.04},
  {46.89,80.09},{38.43,73.38},{38.41,89.85},{61.34,115.62},
  {84.10,152.68},{85.73,138.86},{99.31,180.46},{41.89,75.69},
  {57.75,114.56},{61.00,106.40},{27.91,74.37},{44.89,77.89},
  {82.90,150.13},{ 8.79,34.71},{45.39,82.27},{67.90,106.22},
  {58.54,89.55},{ 1.28,38.65},{61.34,95.43},{ 4.06,22.07},
  {79.91,134.58},{26.41,52.58},{ 7.32,50.59},{61.64,127.01},
  { 1.49,22.00},{48.10,93.04},{70.92,118.66},{ 6.77,25.81},
  {88.64,152.57},{38.34,66.50},{ 8.44,29.80},{26.53,63.70},
  {80.60,130.91},{25.51,49.06},{16.28,47.94},{ 1.62,30.14},
  {76.19,148.14},{11.74,37.77},{40.61,91.98},{10.29,43.36},
  {39.06,74.98},{94.94,174.45},{45.83,91.54},{15.37,34.57},
  {80.66,164.06},{77.15,138.65},{97.42,182.10},{28.34,61.01},
  {89.49,157.67},{88.71,168.13},{56.25,129.47},{68.56,129.26},
  {50.82,100.33},{15.98,53.67},{ 3.82,17.26},{66.46,127.55},
  {12.21,55.00},{86.82,148.57},{48.40,93.56},{69.01,113.72},
  {10.73,37.92},{92.86,146.43},{18.42,64.39},{40.56,85.90},
  {72.17,125.52},{67.59,131.81},{33.09,75.00},{15.20,42.77},
  { 9.32,39.95},{ 8.47,30.12},{14.41,72.50},{81.61,141.76},
  {46.74,92.74},{78.90,139.34},{31.47,75.34},{59.16,119.93},
  {21.87,77.64},{84.37,155.25},{58.94,98.47},{69.43,122.46},
  {66.09,122.03},{47.30,90.93},{74.03,134.25},{18.83,55.97},
  {46.35,95.84},{ 2.40,11.25},{91.24,173.20},{57.86,94.19},
  {47.81,92.90},{64.49,122.06},{44.87,92.55},{92.13,164.69},
  {71.84,131.12},{98.25,167.84},{23.27,62.17},{75.75,115.80},
  {52.87,89.64},{24.59,62.05},{84.49,148.46},{63.80,136.28},
  {32.45,66.16},{61.44,113.25},{ 6.85,47.24},{85.90,153.41},
  {77.78,163.70},{65.64,108.72},{89.95,172.76},{46.85,77.87},
  {38.88,84.07},{97.24,175.76},{62.96,125.11},{11.45,30.96},
  {46.90,94.05},{10.69,42.63},{60.38,120.13},{27.31,66.44},
  {78.59,125.03},{30.27,67.80},{ 8.90,40.30},{75.40,140.11},
  {90.96,160.01},{29.59,58.18},{59.93,120.77},{93.30,164.66},
  {24.85,82.51},{60.27,116.89},{38.49,64.89},{98.82,159.93}
};
