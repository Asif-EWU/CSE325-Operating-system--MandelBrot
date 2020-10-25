#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#define MX 10000

struct thread_args
{
    struct bitmap *b;
    double xmin, xmax, ymin, ymax;
    int itermax, start, end;
};

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void* compute_image( void* args );

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>   X coordinate of image center point. (default=0)\n");
	printf("-y <coord>   Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>   Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels>  Width of the image in pixels. (default=500)\n");
	printf("-H <pixels>  Height of the image in pixels. (default=500)\n");
	printf("-o <file>    Set output file. (default=mandel.bmp)\n");
	printf("-n <threads> Number of threads. (default=1)\n");
	printf("-h           Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;
	int i, x, var, rc;
	pthread_t p[MX];

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int    thread = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:n:o:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
            case 'n':
				thread = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

    // x variable stores the number of image_height loop to run per thread
	x = (image_height / thread);
	if(image_height % thread != 0) x++;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

    var = 0;
	for(i=0; i<thread; i++)
	{
        //Create thread_args arguments
        struct thread_args *temp = (struct thread_args *) malloc(sizeof(struct thread_args));
        temp -> b       = bm;
        temp -> xmin    = xcenter-scale;
        temp -> xmax    = xcenter+scale;
        temp -> ymin    = ycenter-scale;
        temp -> ymax    = ycenter+scale;
        temp -> itermax = max;
        temp -> start   = var;
        if(var + x > image_height) var = image_height;
        else var = var + x;
        temp -> end     = var - 1;

        // Compute the Mandelbrot image in threads
        rc = pthread_create(&p[i], NULL, compute_image, temp);
        if (rc)
        {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	// wait for each thread to complete
	for(i=0; i<thread; i++) pthread_join(p[i], NULL);

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

//void compute_image( struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max )
void* compute_image( void* args )
{
	int i, j, max, start, end;
	double xmin, xmax, ymin, ymax;

	struct thread_args *st = (struct thread_args *) args;
    struct bitmap *bm = st -> b;
	int width = bitmap_width(bm);
	int height = bitmap_height(bm);
	xmin  = st -> xmin;
	xmax  = st -> xmax;
	ymin  = st -> ymin;
	ymax  = st -> ymax;
	max   = st -> itermax;
	start = st -> start;
	end   = st -> end;

	// For every pixel in the image...

	for(j=start; j<=end; j++) {

		for(i=0; i<width; i++) {

			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			bitmap_set(bm,i,j,iters);
		}
	}

	// free the struct used for arguments
	free(st);

    return 0;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}

