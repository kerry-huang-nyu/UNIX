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


//Constants 
size_t DEFAULT_ROWS = 10;
size_t DEFAULT_COLUMNS = 10;
size_t DEFAULT_GENERATIONS = 10;


// size_t live_neighbors(int i, int j){
//     size_t live_neighbors = 0; 
//     for (int delta_i = -1, delta_i < 2; delta_i ++){
//         for (int delta_j = -1, delta_j < 2; delta_j ++){
//             int newi = delta_i + i; 
//             int newj = delta_j + j; 
            
//             //skip the case where you are checking the self 
//             if (newi == i && newj == j){ 
//                 continue ; 
//             }

//             if (0 <= newi <  && 0 <= newj < ){
//                 live_neighbors += thing[newi][newj]; 
//             }
//         }
//     }
// }

// //modify the pointer and memory to simulate the next iteration 
// void runIteration(){ 

//     //use an array to store the next state 

//     //for each index within the array try to do this 
//     for (size_t i = 0; i < ; i ++){
//         for (size_t j = 0; j <; j ++){ 
//             //look to the 8 surrounding and find the number on the sides 

//         }
//     }

//     //copy from local array to heap array 
//     for (size_t i = 0; i < ; i ++){
//         for (size_t j = 0; j <; j ++){
//             heap[i][j] = local[i][j];
//         }
//     }

// }

// void parseCmdline(){

// }

// (void*).....  createInitialState(){

//     //parse and then create the initial state of the stuff 

// }

// //delete the state 
// () ... cleanup(){
    

// }

// //print the initial state of the thing 
// void... printState(void* ){ 

// }


int main() {

    // parseCmdline(); 
    // void* thing = createInitialState();
    // for (size_t i = 0; i < generations; i++){
    //     runIteration(); 
    //     //output message 
    //     print("hello this is iteration ", i )
    //     printState(); 
    // }
    // cleanup(); 

    puts("hello world"); 

    return 0; 

}