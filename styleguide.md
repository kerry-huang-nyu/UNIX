
# String Manipulation
### converting strings into integers: 
use strtol instead of atoi 
**check for errors 
``` c
long int strtol(const char *nptr, char **endptr, // first non-digit
int base); 
```

### copy string 
char *strncpy(char *destination, const char *source, size_t num);

### duplicating strings 
allocates more space on the heap! automaticaly 
strdup 


### 


### zero a memory 
use memset with int = 0 
``` c
memset(void *b, int c, size_t n) 
``` 


### dynamic memory 
We need to cast into a pointer and also do arithmetic 
Must check if the returned memory is not NULL 
``` c
void *malloc(size_t size); 
``` 



