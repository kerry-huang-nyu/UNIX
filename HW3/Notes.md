
** how does the file directory get traversed?** 

** does the order matter for the display ** 

** how does multiple references to hard links change things ** 


Task:
Implement the utility du.

If no arguments, then do du on the current working directory.
Otherwise, the argument (you only have to handle one) is a directory to run du on.
Support the -a option to be able to see the individual files and their sizes.

Note what happens when the du command is run and there is
a file that is linked to (has a hard link) more than once from within the directory tree.

a soft / symbolic link.


Turn in:
A makefile
The source file du.c
Optional README. If there is anything I should know about, such as features that don't work.



What does du do?
reports in kilobytes -> so then i guess a file automatically is going to be in kilobytes instead of bytes 


Hard links: 
To create a hard link, use the ln command in terminal (ln <original_file> <link_name>). Hard links create a new file name pointing to the same data (inode) as the original, allowing either file to be deleted without removing the data, provided at least one link exists.

cannot make hard link for directories 


what happens when we have a hard and soft link? 



use readdir to try to read the entire directory + you prolly don't have that much control anyway 
-> keep the first instance then discard the rest 

double and realloc and stuff. array on the heap to store all seen inodes 

lstat holds the number of links that the files have 


the number at the start is in blocks -> and a block is whatever kilobytes or soemthing depedns on the systemm 

