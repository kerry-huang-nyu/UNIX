/*
Rules: 
Any live cell with fewer than two live neighbors dies.
Any live cell with two or three live neighbors lives on to the next generation.
Any live cell with more than three live neighbors dies.
Any dead cell with exactly three live neighbors becomes a live cell.


Command Line: 
life rows(10) columns(10) filename(life.txt) generations(10)

Test the standard output with just life 

Requirements: 
1. do not use variable sized arrays 
2. do not use local arrays to represent the world, so use the heap instead (since we will provide the size)
3. maybe do some error checking as well for the input 
4. edges are always marked dead 

*/

#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 
#include <string.h> 
#include <errno.h> 


//Constants 
size_t DEFAULT_ROWS = 10;
size_t DEFAULT_COLUMNS = 10;
size_t DEFAULT_GENERATIONS = 10;

size_t countLiveNeighbors(const bool** universe, int i, int j, int rows, int columns);
void runIteration(bool** universe, int rows, int columns);
void parseCmdline(int argc, char* argv[], int* rows, int* columns, int* generations, char** filename);
bool** createInitialState(int rows, int columns);
void cleanup(bool** universe, int rows, int columns);
void printState(const bool** universe, int rows, int columns);
void readfile(bool** universe, int rows, int columns, const char* filename);
int parseInt(char* start);

//calculate the number of live neighbors next to a cell
size_t countLiveNeighbors(const bool** universe, int i, int j, int rows, int columns){
    size_t liveNeighbors = 0; 
    for (int delta_i = -1; delta_i <= 1; delta_i ++){
        for (int delta_j = -1; delta_j <= 1; delta_j ++){
            int newi = delta_i + i; 
            int newj = delta_j + j; 
            
            //skip the case where you are checking the self 
            if (newi == i && newj == j){ 
                continue ; 
            }

            if (0 <= newi && newi < rows && 0 <= newj && newj < columns){
                liveNeighbors += universe[newi][newj]; 
            }
        }
    }
    return liveNeighbors; 
}

//modify the pointer and memory to simulate the next iteration 
void runIteration(bool** universe, int rows, int columns){ 
    //use an array to store the next state 
    bool** tempMatrix = createInitialState(rows, columns); 

    //for each index within the array try to do this 
    for (size_t i = 0; i < rows; i ++){
        for (size_t j = 0; j < columns; j ++){ 
            //look to the 8 surrounding and find the number on the sides 
            size_t liveNeighbor = countLiveNeighbors(universe, i, j, rows, columns);

            //copy over the status of the universe into the local tempMatrix
            tempMatrix[i][j] = universe[i][j]; 
            
            if (tempMatrix[i][j]){
                if (liveNeighbor < 2){
                    tempMatrix[i][j] = false; 
                }
                else if (liveNeighbor <= 3){
                    tempMatrix[i][j] = true; 
                }
                else {
                    tempMatrix[i][j] = false;
                }
            }
            else {
                if (liveNeighbor == 3){
                    tempMatrix[i][j] = true; 
                }
            }
        }
    }

    //copy from local array to universe 
    for (size_t i = 0; i < rows; i ++){
        for (size_t j = 0; j < columns; j ++){
            universe[i][j] = tempMatrix[i][j];
        }
    }

    cleanup(tempMatrix, rows, columns);

}

int parseInt(char* start){
    errno = 0;
    char * endptr = NULL;

    long value = strtol(start, &endptr, 10);
    if (errno == 0 && endptr != start){
        if ((int) value >= 0){
            return (int) value;
        }
        else {
            puts("Error: values cannot be under 0. Reverting to default."); 
        }
    }
    else{
        puts("Error: unknown error. Reverting to default.");
    }
    return DEFAULT_ROWS; 
}

void parseCmdline(int argc, char* argv[], int* rows, int* columns, int* generations, char** filename){
    //skip the first assignment   

    if (1 < argc){
        *rows = parseInt(argv[1]);
    }

    if (2 < argc){
        *columns = parseInt(argv[2]);
    }

    if (3 < argc){
        *generations = parseInt(argv[3]);
    }

    if (4 < argc){
        *filename = argv[4];
    }

    if (5 < argc) {
        puts("Error: too many arguments. Ignoring extra arguments"); 
    }
}

//return a 2d world for booleans 
bool** createInitialState(int rows, int columns){ 
    bool** world = (bool**) malloc(sizeof(bool*) * rows); 

    for (size_t i = 0; i < rows; i++){
        bool* newRow = (bool*) malloc(sizeof(bool) * columns);
        memset(newRow, false, columns); //set all locations to be false 
        world[i] = newRow; 
    }

    //parse and then create the initial state of the stuff 
    return world;
}

//delete the state 
void cleanup(bool** universe, int rows, int columns){
    for (int i = 0; i < rows; i ++){
        free(universe[i]);
        universe[i] = NULL;
    }
    free(universe); 
    universe = NULL;
}

//print states of the universe of items
void printState(const bool** universe, int rows, int columns){ 
    for (size_t i = 0; i < rows; i ++){
        for (size_t j = 0; j < columns; j ++){
            if (!universe[i][j]){
                printf("-");
            }
            else {
                printf("*"); 
            }
        }
        printf("\n");
    }

}


void readfile(bool** universe, int rows, int columns, const char* filename){
    FILE * lifeFile = fopen(filename, "r"); 
    if (lifeFile == NULL){
        perror("fopen failed"); 
    }
    
    //columns + 1 to accomodate for end of line 
    char* line = NULL;
    size_t lineLen = 0; //the size of the buffer not the line read 
    ssize_t read; //the number actually read 
    
    size_t i = 0; 
    while ((read = getline(&line, &lineLen, lifeFile)) != -1 && i < rows){
        for (size_t j = 0; j < columns && j < read; j ++){
            if (line[j] == '*'){
                universe[i][j] = true; 
            }
        }

        i ++; 
    }

    int status = fclose(lifeFile);
    if (status != 0){
        perror("fclose failed");
    }

    free(line); 

}


int main(int argc, char* argv[]){

    int rows = DEFAULT_ROWS;
    int columns = DEFAULT_COLUMNS;
    int generations = DEFAULT_GENERATIONS;
    char* filename = NULL;

    parseCmdline(argc, argv, &rows, &columns, &generations, &filename); 
    void* universe = createInitialState(rows, columns);
    if (filename){
        readfile(universe, rows, columns, filename); 
    }

    for (size_t i = 0; i < generations + 1; i++){
        //output message 
        printf("Generation %zu:\n", i);
        printState(universe, rows, columns); 
        printf("================================\n");

        runIteration(universe, rows, columns); 
    }

    //cleanup memory even though we are exiting the program immediately
    cleanup(universe, rows, columns); 
    //puts("ended main loop"); 

    return 0; 

}