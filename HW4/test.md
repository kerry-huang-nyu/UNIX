ls 
ls -l shell.dSYM
wc < test.md
ls > output.txt 
echo hello >> appendoutput.txt
ls nonexistentfile 2> errors.txt
< input.txt wc
grep hello < test.md > output.txt

$? 
    0: true
    1: false 
    127: command not found 
    128 + signal: signal termination 
    