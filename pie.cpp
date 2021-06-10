#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Pie

int computePartialSum(int parentP[], int childP[]);

int main(int argc, char *argv[]) {


    int N = atoi(argv[1]); // Iterations for PI
    int T = atoi(argv[2]); // Processes created in child

    int parentP[T][2];
    int childP[T][2];

    if (argc > 3) {
        printf("Error: Too many arguments, must be 3!\n");
        exit(3);
    }
    else if (argc < 3) {
        printf("Error: Too few arguments, must be 3!");
        exit(3);
    }

    if (N < T) {
        printf("Error: First parameter must be greater than second\n");
        exit(1);
    }
    int pid;
    for (int j  = 0; j < T; j++) { // Dynamically create pipes and childs
        
        int r = pipe(parentP[j]);
        r = pipe(childP[j]);
        pid = fork();
        
        if (pid < 0) { // Error
            printf("Error: Fork failed!");
            exit(3);
        }
        else if (pid > 0) { // parent process
            close(parentP[j][0]);
            write(parentP[j][1], &N, sizeof(N));
            write(parentP[j][1], &T, sizeof(T));
            write(parentP[j][1], &j, sizeof(j));
            close(parentP[j][1]);
        }
        else if (pid == 0) { // Child

            computePartialSum(parentP[j],childP[j]); // We pass in pipe arrays with just j to the function, we then read j from the pipe

        }
    }
    
    double accumulator = 3.0;
    double pi = 0.0;
    for (int j = 0; j < T; j++) {
        
        close(childP[j][1]);
        read(childP[j][0], &pi, sizeof(pi));
        close(childP[j][0]);

        accumulator += pi;
    }

        printf("\nThe approximation of pi with N=%d and T=%d processes is %1.16lf\n", N, T, accumulator);

        for (int i = 0; i <= T; i++) { // parent waits for each child process created (T = # of children)
            waitpid(-1, NULL, 0);
        }

        exit(0);
}

int computePartialSum(int parentP[], int childP[]) { 

    int n, t, j;

    close(parentP[1]);
    //wait(NULL);

    read(parentP[0], &n, sizeof(n)); // Read values from pipe for the algorithm
    read(parentP[0], &t, sizeof(t));
    read(parentP[0], &j, sizeof(j));
    close(parentP[0]);

    // ------------------ approximation PI ---------------------
    double k = 1;   // signal for next operation
    double pi = 0.0;

    for (double i = n * j / t + 1.0; i <= ((n / t) * (j + 1.0) * 2.0); i++) {

        pi += k * 4.0/((2.0 * i) * (2.0 * i + 1.0) * (2.0 * i + 2.0));
        k = -k;
    }

    //printf("\nApproximated value of PI = %1.16lf\n", pi);

    // ----------------------------------------------------------

    close(childP[0]);
    write(childP[1], &pi, sizeof(pi));
    close(childP[1]);
    

    exit(0);
}