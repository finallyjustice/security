#include <stdio.h>

int main(int argc, char **argv)
{
	char shellcode[1024];
	
	puts("Please input the message:");
	gets(shellcode);
	
	void (*func)();
	func = (void (*)())shellcode;
	(void)(*func)();
	
	return 0;
}
