#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

bool parse_options(int argc, char *argv[], bool *ignore_env,
                   int *first_non_option_index);

char **build_environment(char *argv[], int start_index,
                         bool ignore_env);

void print_environment(char *const *env);

char **get_variable(char *variable, char **env);

void execute_command(char *command, char **command_argv);

void copy_env(char **newenv, char **oldenv); 

void append_or_modify_env(char *argv[], int start_index, int end_index, char **env);

int main(int argc, char *argv[]) {
  // ignore environment -i option
  bool ignore_env = false; 

  // start at the first index since the first is ./env
  int index = 1;           

  /* Parse -i option */
  if (!parse_options(argc, argv, &ignore_env, &index)) {
    fprintf(stderr, "Invalid options\n");
    return EXIT_FAILURE;
  }

  /* Build environment */
  char **new_env = build_environment(argv, index, ignore_env);
  environ = new_env; 
  
  int command_index = index;

  /* If no command, print environment */
  // index is modified to be the first value that is does not contain a = 
  if (command_index >= argc) {
    print_environment(new_env);
    return EXIT_SUCCESS;
  }
  
  /* Execute command */
  execvp(argv[command_index], &argv[command_index]);

  /* If exec fails */
  perror("exec");
  return EXIT_FAILURE;
}

bool parse_options(int argc, char *argv[], bool *ignore_env, int *index) {
  const char* IGNORE_STRING = "-i"; 
  // detect -i
  if (1 < argc && strcmp(argv[1], IGNORE_STRING) == 0) {
    //need to update index consuming 1 string 
    *ignore_env = true;
    (*index)++;
  }

  // detect if you can find an = sign in the result, indicating it is an option
  // provided
  while (*index < argc) {
    // '=' doesn't exist in the string
    if (strchr(argv[*index], '=') == NULL) {
      break;
    }

    (*index)++;
  }

  return true;
}

void append_or_modify_env(char *argv[], int start_index, int end_index, char **env) {
  //start index is the index where arguments where = exists starts 
  //end index is the index where exec arguments start 
  
  for (int i = start_index; i < end_index; i++) {
    // find the entry
    char *assignment = argv[i];
    char *eq = strchr(assignment, '=');
    
    if (eq == NULL){
      perror("Weird error we shouldn't fail to find '=' within arguments"); 
      exit(EXIT_FAILURE);
    }
    size_t keylen = eq - assignment;
    char *key = malloc(keylen + 1);
    
    if (key == NULL){
      perror("No More Memory");
      exit(EXIT_FAILURE);
    }
    //copy and null terminate the key 
    strncpy(key, assignment, keylen);
    
    char **location = get_variable(key, env);
    free(key);

    // modify
    if (*location != NULL) {
      free(*location);
      // free first
    }
    *location = (char*) malloc(sizeof(char) * (strlen(argv[i]) + 1));
    if (*location == NULL){
      perror("No More Memory");
      exit(EXIT_FAILURE);
    }

    //automatically null terminates with a \0 padding until the end 
    strncpy(*location, argv[i], strlen(argv[i]));
  }
}

void copy_env(char **newenv, char **oldenv) {
  // assumes that env is going to be large enough to accomodate all the previous
  // environment's information

  for (char **ptr = oldenv; *ptr != NULL; ptr++) {
    *newenv = (char *)malloc(sizeof(char *) * (strlen(*ptr) + 1));
    if (*newenv == NULL){
      perror("Error allocating space");
      exit(EXIT_FAILURE); 
    }
    strncpy(*newenv, *ptr, strlen(*ptr));
    newenv += 1;
  }
}

char **build_environment(char *argv[], int end_index,bool ignore_env) {

  char **env = NULL;
  int start_index = 1;
  int newsize;

  if (ignore_env) {
    // ditch env
    start_index++;
    newsize = end_index - start_index;
  } else {
    if (start_index >= end_index) {
      return environ;
    }

    // calculate environ_size
    int environ_size = 0;
    for (char **ptr = environ; *ptr != NULL; ptr++) {
      environ_size++;
    }
    newsize = end_index - start_index + environ_size;
  }

  // malloc the new sizes
  env = (char **) malloc(sizeof(char *) * (newsize + 1));
  if (env == NULL){
    perror("Error allocating space"); 
    exit(EXIT_FAILURE); 
  }
  memset(env, 0, newsize + 1); //null terminate 

  // make copies
  if (!ignore_env) {
    copy_env(env, environ);
  }

  // make changes or appends
  append_or_modify_env(argv, start_index, end_index, env);

  return env; 
}

void print_environment(char *const *env) {
  for (char *const * envptr = env; *envptr != NULL; envptr++) {
    puts(*envptr);
  }
}

char **get_variable(char *variable, char **env) {
  // take the environment and search whether the variable is within the
  // environment
  size_t keylen = strlen(variable);
  char **ptr = env;

  //if the variable 
  for (; *ptr != NULL; ptr++) {
    if (strncmp(*ptr, variable, keylen) == 0 && strlen(*ptr) > keylen && (*ptr)[keylen] == '=') {
      return ptr;
    }
  }

  return ptr;
}


/*
This is my attempt at implementing execvpe 
// void execute_command(char **env, char *command, char **command_argv) {
//   // execvp()

//   // get the path variable
//   char *PATH = "PATH";

//   const char *path = *(get_variable(PATH, env));

//   // first of all remove the path from the start when you dup
//   char *mypath;

//   if (path) {
//     path += strlen(PATH) + 1; // for the \n character
//     mypath = strdup(path);
//   } else {
//     const char *DEFAULT_PATH =
//         "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
//     mypath = strdup(DEFAULT_PATH);
//   }

//   // execute in the current directory and not anywhere else

//   execve(command, command_argv, env);

//   // process all path variables
//   char *token = strtok(mypath, ":");
//   // make something just become a '\0'

//   while (token != NULL) {
//     // try to execve for each using the command
//     int tokenlen = strlen(token);
//     int commandlen = strlen(command);

//     char *fullpath =
//         (char *)malloc(sizeof(char) * strlen(token) + 1 + strlen(command) + 1);

//     strcpy(fullpath, token);
//     fullpath[tokenlen] = '/';
//     strcpy(fullpath + tokenlen + 1, command);
//     fullpath[tokenlen + commandlen + 1] = '\0';
//     // todo don't forget to null terminate

//     // printf("%s\n and this is how long %lu", fullpath, strlen(fullpath));

//     execve(fullpath, command_argv, env);
//     token = strtok(NULL, ":");

//     free(fullpath);
//   }
// }
*/