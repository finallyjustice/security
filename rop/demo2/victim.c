// gcc -fno-stack-protector -o victim victim.c

#include <stdio.h>
int main() 
{
	char name[64];
	printf("%p\n", name);  // Print address of buffer
	fflush(stdout);
	puts("What's your name?");
	fflush(stdout);
	gets(name);
	printf("Hello, %s!\n", name);
	return 0;
}
