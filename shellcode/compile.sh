gcc -fno-stack-protector run_shell.c -o run_shell
execstack -s run_shell
