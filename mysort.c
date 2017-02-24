#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <string.h>


char** start;
int size;
int capacity;

#define BUFFER 1024
#define STARTCAPACITY 2


/**
* Prints the array in either sorted or reverse sorted order.
* Only displays first n lines as passed in by user.
*
* @param ptr ptr to the start of the words
* @param rflag whether to reverse or not
* @param n number of lines to print
* @param pt points to an int vairbale that keeps track of which
*/
void print(char** ptr, int rflag, int n){

    if(n == -1){
        n = size;
    }

    //normal print
    if(rflag !=1) {
        int i;
        for (i = 0; i < n; i++) {
            printf("%s\n", ptr[i]);
        }
    }//reverse order print
    else{
        n = size - n;
        int i;
        for (i = size-1; i >= n; i--) {
            printf("%s\n", ptr[i]);
        }
    }
}

/**
* Used in qsort.  Compares two strings.
*
*
* @param a string one
* @param b string two
*
* @return >0 if a >b, 0 if strings are equal, and <0 if
* a < b
*/
int cmpstr(const void* a, const void* b)
{
    char* aa = *(char**)a;
    char* bb = *(char**)b;
    return strcmp(aa,bb);
}

/**
* This method frees all dynamically allocated memory.
*/
void freeArray(){

    //free all dynamic memory
    int i;
    for (i = size -1 ; i >= 0 ; i--)
    {
        free(start[i]);
    }

    free(start);
}

/**
* This method is called when the array of strings is full.
* It doubles the memory allocated for it by calling realloc.
*
* @param ptr ptr to the array of strings
*
* @return returns a pointer to the new reallocated block
 * of memory.
*/
char** resize(char** ptr){

    //double capacity
    int newCap = capacity * 2;

    char** tmp = realloc(ptr, ((sizeof(char*)) * newCap));

    //make sure realloc worked
    if(ptr == NULL){
        fprintf(stderr, "Could not allocate more memory:, %s\n", strerror(errno));
        exit(1);
    }

    capacity = newCap;

    return tmp;
}



/**
* Main first does command line argument checking, and then proceeds to
*store the strings passed in dynamically as it needs more memory.
*
* @param argc number of args
* @param argv array of args passed in
*
* @return 0 if sucessful, 1 if error happens.
*/
int main(int argc , char* argv[]) {

    //initialize with original capc
    start = malloc(sizeof(char*) * STARTCAPACITY);
    capacity = STARTCAPACITY;
    size = 0;

    //fields
    int rflag = 0;
    int fileRead  = 0;
    int n = -1;
    char ch;
    char *eptr;
    FILE *inFile;
    FILE* source;

    //Read in command line arguments
    while ((ch = getopt(argc, argv, "rn:")) != -1) {
        switch(ch){
            case 'r':
                rflag = 1;
                break;
            case 'n':
                if (strlen(optarg) == 1 && (strcmp(optarg, "0") == 0)){
                    n = 0;
                }else{
                    n = strtol(optarg, &eptr, 10);
                    if(n <= 0){
                        fprintf(stderr, "Not a valid number input: %s, %s\n", optarg, strerror(errno));
                        exit(1);
                    }
                }
                break;
            case '?':

                fprintf(stderr, "Not a valid command line arg: %s, %s\n", optarg, strerror(errno));
                exit(1);

            default:
                break;
        }
    }

    /* Arg command checks*/
    if(argc > 1) {
        //check to see if user passed in file
        inFile = fopen(argv[argc - 1], "r");
        if (inFile != NULL) {
            fileRead = 1;
        }

        //if too many files passes in
        if (argc - optind > 1) {
            fprintf(stderr, "Too many arguments passed in");
            exit(1);
        }

        //if try to pass file and is invalid
        if ((argc - optind  > 0)) {
            if (fileRead == 0) {
                fprintf(stderr, "Invalid file/cannot read: %s\n", strerror(errno));
                exit(1);
            }
        }

    }


    //determine whether reading from stdin or file
    if (fileRead) {
        source = inFile;

    } else {
        source = stdin;
    }

    //while loop fields
    char lineArray[BUFFER];
    while(fgets(lineArray, BUFFER, source) != NULL) {

        //if full allocate more memory
        if(size == capacity){
            start = resize(start);
        }

        //make room for line
        start[size] = (char *) malloc(sizeof(lineArray) + 1);

        //remove end of line char and add to array
        char* line = lineArray;
        strtok(line, "\n");
        strcpy(start[size], line);

        size++;
    }

    //make sure n is within bounds
    if(n > size){
        fprintf(stderr, "Number of lines to print is greater than the amount of lines passed in");
        exit(1);
    }

    //sort and print
    qsort(start, size, sizeof(char*) , cmpstr);
    print(start, rflag, n);

    //free all dynamic memory and files
    freeArray();
    fclose(source);


    return 0;
}