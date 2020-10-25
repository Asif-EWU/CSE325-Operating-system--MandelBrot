#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <pthread.h>
#define MAX_ITERATION 100
#define M 20

int running_process = 0;
float zoom[MAX_ITERATION + 5];
char s_x[] = "0.28790";
char s_y[] = "0.0152";
char s_m[] = "600";
char s_h[] = "1000";
char s_w[] = "1000";
char s_s[M];
char s_n[M];

void make_zoom_array()
{
    int i;
    float initial_zoom = 2.0, final_zoom = 0.00005, multiply;
    multiply = exp(log(final_zoom / initial_zoom) / 101.0);

    // pre-calculate and save all the -s values in an array
    zoom[1] = initial_zoom;
    for(i=2; i<=MAX_ITERATION; i++) zoom[i] = zoom[i-1] * multiply;
}

int main(int argc, char* argv[])
{
    // Check if the number of arguments is valid
    int _p = MAX_ITERATION, _n = 1;

    if(argc != 1 && argc != 3 && argc != 5)
    {
		fprintf(stderr,"mandelmovie.c : ERROR! Invalid number of arguments.\n");
		exit(0);
	}

	if(argc > 1)
	{
        if(argv[1][1] == 'n')
        {
            _n = atoi(argv[2]);
            if(argc == 5) _p = atoi(argv[4]);
        }
        else
        {
            _p = atoi(argv[2]);
            if(argc == 5) _n = atoi(argv[4]);
        }
	}
    sprintf(s_n, "%d", _n);                     // convert (int) _n to (string) s_n

    char c;
    int i, limit = _p;
    pid_t pid;
    make_zoom_array();

    for(i=1; i<=MAX_ITERATION; i++)
    {
        if(running_process == limit)            // if number of running processes reach the limit value, wait for a process to end
        {
            wait(NULL);
            running_process--;
        }
        if(running_process < limit)             // if number of running process is less than limit value, create another process
        {
            running_process++;
            pid = fork();
        }

        if(pid == -1)
        {
            printf("Error! Failed to create %dth child.\n", i);
            exit(1);
        }
        else if(pid == 0)                       // child process
        {
            char s_name[M], s_zoom[M];
            sprintf(s_name, "m%d.bmp", i);      // make the bmp file name
            sprintf(s_zoom, "%f", zoom[i]);     // convert (float) into (string)
            execlp("./mandel", "./mandel", "-x", s_x, "-y", s_y, "-s", s_zoom, "-W", s_w, "-H", s_h, "-m", s_m, "-n", s_n, "-o", s_name, NULL);
        }
    }

    while(running_process--) wait(NULL);        // wait for all the child process to terminate before terminating main process
    system("ffmpeg -i m%d.bmp mandel.mpg");     // make a video with all the images created through the program
    printf("Done.\n");

    return 0;
}

