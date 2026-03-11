#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>

/* Function prototypes */
void file_open_dup(char* filename, int target_fileno, int format);
char **build_argv(char *line, int *argc);
void parse_redirection(char **argv);
char **strip_redirection_tokens(char **argv, int *argc);
int execute(int argc, char* argv[]);

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

    int last_status = 0; 

    while (true){
        //get a line of input

        printf("%s", prompt_message); 

        //have a prompt string loop so that you can have the prompt overall 
        ssize_t len = getline(&line, &size, stdin);
        if (len == -1) {  // EOF error checking 
            break;
        }

        int argc;
        char ** argv = build_argv(line, &argc);

        if (argc == 0) {
            continue;
        }

        char *command = argv[0];

        if (strcmp(command, "exit") == 0){
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(command, "cd") == 0){
            last_status = 0; //for sucess 
            char *dir = argv[1];
            if (dir == NULL) {
                dir = getenv("HOME");
            }
            if (chdir(dir) < 0){
                fprintf(stderr, "chdir %s failed: %s\n", dir, strerror(errno));
                last_status = 1; 
            }
        }
        else {
            if (strcmp(command, "$?") == 0) {
                printf("%d\n", last_status);
            }
            last_status = execute(argc, argv);
        }
        free(argv); 
    }

    return 0; 
} 



void file_open_dup(char* filename, int target_fileno, int format){
    int fd;

    if (filename != NULL) {
        fd = open(filename, format, 0644);
        if (fd < 0) {
            fprintf(stderr, "open input failed: %s (errno=%d)\n", strerror(errno), errno);
            _exit(EXIT_FAILURE);
        }
        if (dup2(fd, target_fileno) == -1){
            fprintf(stderr, "dup2 failed: %s (errno=%d)\n", strerror(errno), errno);
            _exit(EXIT_FAILURE);
        }
        close(fd);
    }
    else {
        perror("file open failure"); 
    }
}

char **build_argv(char *line, int *argc) {

    int capacity = 4;
    char **argv = malloc(sizeof(char*) * capacity);
    if (argv == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    *argc = 0;

    char *token = strtok(line, " \n");

    while (token != NULL) {

        if (*argc >= capacity) {
            capacity *= 2;
            char **tmp = realloc(argv, sizeof(char*) * capacity);
            if (tmp == NULL) {
                perror("realloc failed");
                free(argv);
                exit(EXIT_FAILURE);
            }
            argv = tmp;
        }

        argv[*argc] = token;
        (*argc)++;

        token = strtok(NULL, " \n");
    }

    char **tmp = realloc(argv, sizeof(char*) * (*argc + 1));
    if (tmp == NULL) {
        perror("realloc failed");
        free(argv);
        exit(EXIT_FAILURE);
    }
    argv = tmp;
    argv[*argc] = NULL;

    return argv;
}

void parse_redirection(char **argv) {

    for (int i = 0; argv[i] != NULL; i++) {

        if (strcmp(argv[i], "<") == 0) {
            file_open_dup(argv[i+1], STDIN_FILENO, O_RDONLY);
        }

        else if (strcmp(argv[i], ">") == 0) {
            file_open_dup(argv[i+1], STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
        }

        else if (strcmp(argv[i], ">>") == 0) {
            file_open_dup(argv[i+1], STDOUT_FILENO, O_WRONLY | O_CREAT | O_APPEND);
        }

        else if (strcmp(argv[i], "2>") == 0) {
            file_open_dup(argv[i+1], STDERR_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
        }

        else if (strcmp(argv[i], "&>") == 0) {
            file_open_dup(argv[i+1], STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
            file_open_dup(argv[i+1], STDERR_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
        }
    }
}

char **strip_redirection_tokens(char **argv, int *argc) {

    int newc = 0;
    int capacity = *argc;
    char **newv = malloc(sizeof(char*) * capacity);
    if (newv == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *argc; i++) {

        if (strcmp(argv[i], "<") == 0 ||
            strcmp(argv[i], ">") == 0 ||
            strcmp(argv[i], ">>") == 0 ||
            strcmp(argv[i], "2>") == 0 ||
            strcmp(argv[i], "&>") == 0) {

            i++;   // skip filename
            continue;
        }

        newv[newc++] = argv[i];
    }

    char **tmp = realloc(newv, sizeof(char*) * (newc + 1));
    if (tmp == NULL) {
        perror("realloc failed");
        free(newv);
        exit(EXIT_FAILURE);
    }
    newv = tmp;
    newv[newc] = NULL;

    *argc = newc;

    return newv;
}


int execute(int argc, char* argv[]){

    //parse the commands separated from parsing redirections 
    pid_t pid = fork();

    if (pid == -1){
        fprintf(stderr, "fork failed: %s (errno=%d)\n", strerror(errno), errno);
        return -1;
    }

    else if (pid == 0){ // child process
        parse_redirection(argv);

        if (argc == 0) {
            _exit(EXIT_SUCCESS);
        }

        char** stripped_argv = strip_redirection_tokens(argv, &argc);
        execvp(stripped_argv[0], stripped_argv);
        
        free(stripped_argv); 
        fprintf(stderr, "execvp failed: %s (errno=%d)\n", strerror(errno), errno);
        if (errno == ENOENT) {
            _exit(127);   // command not found
        } else {
            _exit(126);   // found but not executable
        }
    }

    else {
        int status = 0;

        if (waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "waitpid failed: %s (errno=%d)\n", strerror(errno), errno);
            return 1;
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } 

        else if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }

        else if (WIFSTOPPED(status)){
            return WSTOPSIG(status); 
        }

        return status;
    }
}
