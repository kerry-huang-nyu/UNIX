#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


void file_open_dup(int target_fileno, int format){
    char* file_name = strtok(NULL, " \n");

    int fd;

    if (file_name) {
        fd = open(file_name, format, 0644);
        if (fd < 0) {
            perror("open input");
            exit(EXIT_FAILURE);
        }
        dup2(fd, target_fileno); //Upon successful completion,
    //  the new file descriptor is
    //  returned.  Otherwise, a
    //  value of -1 is returned and
    //  the global integer variable
    //  errno is set to indicate the
    //  error.
        close(fd);
    }
    else {
        perror("file open failure"); 
    }

}


int count_args(char *line) {

    int argc = 0;
    char *token = strtok(line, " \n");

    while (token != NULL) {

        if (strcmp(token, "<") == 0 ||
            strcmp(token, ">") == 0 ||
            strcmp(token, ">>") == 0 ||
            strcmp(token, "2>") == 0 ||
            strcmp(token, "&>") == 0) {

            strtok(NULL, " \n");   // skip filename
        }

        else {
            argc++;
        }

        token = strtok(NULL, " \n");
    }

    return argc;
}

void populate_argv(char *line, char **argv) {

    int index = 0;
    char *token = strtok(line, " \n");

    while (token != NULL) {

        if (strcmp(token, "<") == 0 ||
            strcmp(token, ">") == 0 ||
            strcmp(token, ">>") == 0 ||
            strcmp(token, "2>") == 0 ||
            strcmp(token, "&>") == 0) {

            strtok(NULL, " \n");   // skip filename
        }

        else {
            argv[index++] = token;
        }

        token = strtok(NULL, " \n");
    }

    argv[index] = NULL;
}

void parse_redirection(char *line) {
    char* token;

    token = strtok(line, " \n");

    while (token != NULL) {

        if (strcmp(token, "<") == 0) {
            file_open_dup(STDIN_FILENO, O_RDONLY);
        }

        else if (strcmp(token, ">") == 0) {
            file_open_dup(STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
        }

        else if (strcmp(token, ">>") == 0) {
            file_open_dup(STDOUT_FILENO, O_WRONLY | O_CREAT | O_APPEND);
        }

        else if (strcmp(token, "2>") == 0) {
            file_open_dup(STDERR_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
        }

        else if (strcmp(token, "&>") == 0) {
            file_open_dup(STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
            dup2(STDOUT_FILENO, STDERR_FILENO);
        }

        else {
            //nothing; 
        }

        token = strtok(NULL, " \n");
    }
}


int execute(char *line, int argc, char* argv[]){

    //parse the commands separated from parsing redirections 
    pid_t pid = fork();

    if (pid == -1){
        perror("fork failed");
        return -1;
    }

    else if (pid == 0){ // child process

        parse_redirection(line);

        if (argc == 0) {
            exit(EXIT_SUCCESS);
        }

        execvp(argv[0], argv);

        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    else {
        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return -1;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } 

        else if (WIFSIGNALED(status)) {
            return WTERMSIG(status) | 0x80;
        }

        return status;
    }
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

    //int status; 

    while (true){
        //get a line of input 
        printf("%s", prompt_message); 

        //have a prompt string loop so that you can have the prompt overall 
        ssize_t len = getline(&line, &size, stdin);
        if (len == -1) {  // EOF error checking 
            break;
        }

        int argc = count_args(line);
        char **argv = (char**) malloc(sizeof(char*) * (argc + 1));
    
        populate_argv(line, argv);

        if (argc == 0) {
            continue;
        }

        char *command = argv[0];

        if (strcmp(command, "exit") == 0){
            exit(EXIT_SUCCESS);
        }

        if (strcmp(command, "cd") == 0){
            char *dir = argv[1];
            if (dir == NULL) {
                printf("a;sldkfja;slkdfj;aslkfj;aslkdfj;aslkdf");
                dir = getenv("HOME");
            }
            if (chdir(dir) < 0){
                fprintf(stderr, "chdir %s failed: %s\n", dir, strerror(errno));
            }
        }
        else {
            int status = execute(line, argc, argv);
            (void)status; // placeholder for $? support
        }
        //support the $? variable as well

    }

    return 0; 
} 
