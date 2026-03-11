ls 
ls -l shell.dSYM
wc < test.md
ls > output.txt 
echo hello >> appendoutput.txt
ls nonexistentfile 2> errors.txt
< input.txt wc
grep hello < test.md > output.txt
ls > hello.txt > bello.txt : should expect to have bello with the ls contents only 
wc < hello.txt < bello.txt : should expect to have wc applied only bello.txt only 
cd > hello.txt : normally bash would create hello.txt but this time we will just expect to change directories with no side effects 


$? 
    0: true
    1: false 
    127: command not found 
    128 + signal: signal termination 
    