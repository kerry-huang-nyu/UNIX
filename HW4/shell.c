
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void execute(){
}

int main(){ 
    // get the default message for the shell 
    char* DEFAULT_MESSAGE = "shell> "; 
    char* DEFAULT_SHELL_ENV = "PS1"; 
    char* prompt_message = getenv(DEFAULT_SHELL_ENV);

    if (prompt_message == NULL){
        prompt_message = DEFAULT_MESSAGE; 
    }

    
    char *line = NULL;
    size_t size = 0;

    while (true){
        //get a line of input 
        printf("%s", prompt_message); 

        //have a prompt string loop so that you can have the prompt overall 
        ssize_t len = getline(&line, &size, stdin);
        if (len == -1) {  // EOF error checking 
            break;
        }

        //strtok to tokenize the line changinging the spaces into \0 
        strtok(line, " "); 
        strtok(line, "\n");

        //exit 
        char* command = line; 
        //printf("line is %s\n", command);
        if (strcmp(command, "exit") == 0){
            exit(EXIT_SUCCESS); 
        }

        //isn't that gonna automatically execute? 
        //no since the other process will execute but not the current 
        if (strcmp(command, "cd") == 0){ 
            chdir(command + 1); 
        }
        
        //take the tokenized commands and execute them  
        else { 
            execute();
        }

        //support the $? variable as well

    }

    return 0; 
} 


