#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <dirent.h>


//maybe we can get the output to the files and then when you read you don't need to create a whole struct
struct output {
    size_t size;
    ino_t ino;
    char *name;   // basename to print
    bool is_dir; 
};


bool vector_search(struct output* vector, size_t vector_size, ino_t target_inode){
    size_t vec_index = 0; 
    for (struct output* ptr = vector; vec_index < vector_size; ptr++){
        if (ptr->ino == target_inode){
            return true; 
        }
        vec_index ++; 
    }

    return false; 
}

//manage the vector, only consider resizing to 2 times the original size 
//no need to size down 
void vector_append(struct output** vector_ptr, size_t* vector_size_ptr, size_t* used_ptr, struct output item){
    //realloc due to insufficient space 
    if (*used_ptr == *vector_size_ptr){
        //size up by 2 
        *vector_size_ptr *= 2; 
        *vector_ptr = (struct output*) realloc(*vector_ptr, sizeof(struct output) * (*vector_size_ptr)); 
    }

    struct output* vector = *vector_ptr; 
    vector[*used_ptr] = item; 
    (*used_ptr) ++; 

}

//dir is going to be able to traverse through the values 
size_t traverse(bool all, char *path, struct output **entries_ptr, size_t *cap_ptr, size_t *used_ptr) {
    DIR *dir = opendir(path);
    if (!dir) { 

        // If it's not a directory, treat it as a file path
        struct stat sb;
        if (lstat(path, &sb) == 0) {
            // hardlink handling
            bool repeat = vector_search(*entries_ptr, *used_ptr, sb.st_ino); 
            size_t sz = (repeat ? 0 : (size_t) sb.st_blocks);

            struct output item = {
                .size = sz,
                .ino = sb.st_ino,
                .name = path,
                .is_dir = false
            };

            if (!item.name) { perror("strdup"); exit(EXIT_FAILURE); }
            
            if (!repeat){
                vector_append(entries_ptr, cap_ptr, used_ptr, item);
            }
                
            return sz;
        }

        // Can't open and can't stat -> ignore or error out
        fprintf(stderr, "opendir/lstat failed on '%s'\n", path);
        return 0;
    }

    size_t dir_total = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;

        // create child_path = path + "/" + name
        size_t len = strlen(path) + strlen(name) + 2; // '/' + '\0'
        char *child_path = malloc(len);
        if (!child_path) { perror("malloc"); exit(EXIT_FAILURE); }

        snprintf(child_path, len, "%s/%s", path, name);

        size_t child_total = traverse(all, child_path, entries_ptr, cap_ptr, used_ptr);
        dir_total += child_total;
    }

    struct output item = {
        .size = dir_total,
        .ino = -1, 
        .name = path, 
        .is_dir = true, 
    };
    vector_append(entries_ptr, cap_ptr, used_ptr, item);

    closedir(dir);
    return dir_total;
}

//given the information to display 
//display based on the stuff according to -a 
//some of the -a will block the display because the files cannot be displayed 
void display(bool all, const struct output* entries, size_t vector_used){ 
    size_t index = 0; 
    for (const struct output* ptr = entries; index < vector_used; ptr ++){
        if (!all && !ptr->is_dir){
            //skip this section 
        }
        else{
            printf("%zu\t%s\n", ptr->size, ptr->name); 
        }
        index ++; 
    }
}

//free all the characters, then free the entries on the heap 
void cleanup(struct output* entries, size_t vector_used){
    size_t index = 0; 
    for (const struct output* ptr = entries; index < vector_used; ptr ++){
        free(ptr->name); 
        index ++; 
    }
    free(entries); 
}


void parse(int args, char* argv[], bool* all_ptr, char** directory_ptr){
    //change the default directory if needed 
    const char* DISPLAY_ALL = "-a"; 

    if (args >= 4){
        perror("Too many options received");
        exit(EXIT_FAILURE); 
    }

    if (args >= 2){

        //check if we have the same 
        if (strcmp(argv[1], DISPLAY_ALL) == 0){
            *all_ptr = true; 
        }
        else {
            *directory_ptr = argv[1]; 
        }
    }

    if (args >= 3){
        *directory_ptr = argv[2]; 
    }

}

int main(int args, char* argv[]){
    bool all = false; 
    char* DEFAULT_DIRECTORY = "."; 
    char* directory = DEFAULT_DIRECTORY; 

    size_t DEFAULT_VECTOR_SIZE = 8; 
    size_t vector_size = DEFAULT_VECTOR_SIZE;
    size_t vector_used = 0; 
    struct output* entries = (struct output*) malloc(sizeof(struct output) * vector_size); 

    //a;sdlkfjas;dfkj;aslfdkj;aslkdfjs;aldkfj;salkdfj;alskdjf;lsajdkf must check for issues here 

    parse(args, argv, &all, &directory); 

    //make a copy such that we don't get the free error 
    directory = strdup(directory); 

    traverse(all, directory, &entries, &vector_size, &vector_used); 

    display(all, entries, vector_used); 

    cleanup(entries, vector_used);
}
